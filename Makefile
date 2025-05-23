BUILD_TARGET ?= //src:freitest
BUILD_CONFIG ?= debug
BUILD_ARGS ?=
RUN_ARGS ?= --log-level=trace \
	--CircuitName medium/benchmark1 \
	--Settings ./settings/circuits/benchmarks/generated-generic.freitest.jsonc \
	--Settings ./settings/workflows/scale4edge/tpg/stuck-at/atpg-stuckat-sat-fullscan.freitest.jsonc
TEST_TARGET ?= $$(${BAZEL} query 'kind("test", //test:*)')
TEST_ARGS ?= --log_level=all

BAZEL ?= /bin/env bazel --client_debug --max_idle_secs=0
BAZEL_BUILD_ARGS ?=
BAZEL_RUN_ARGS ?= --run_under="cd $$(pwd) && "
BAZEL_TEST_ARGS ?= --test_output=all

DOCKER_FILE ?= Dockerfile.ubuntu
DOCKER_TAG ?= latest
DOCKER_TARGET ?= runner
DOCKER_BUILD_TESTS ?= yes
DOCKER_RUN_TESTS ?= yes

.SUFFIXES:
.PHONY: all help build run test docker/build docker/run docker/package docker/image docker/sysroot clean mrproper public

all: help

help::
	@printf "Usage: \033[31mmake <target>\033[39m\n"
	@printf "\n"
	@printf "\033[1mLocal build targets:\033[0m\n"
	@printf "\033[31m\tbuild\033[39m          Compiles the FreiTest executable with Bazel\n"
	@printf "\033[31m\trun\033[39m            Compiles and runs FreiTest with RUN_ARGS arguments\n"
	@printf "\033[31m\ttest\033[39m           Compiles and runs all unit-tests of FreiTest\n"
	@printf "\033[31m\tclean\033[39m          Cleans the temporary files created by Bazel\n"
	@printf "\033[31m\tmrproper\033[39m       Cleans all temporary files from Bazel, even in ~/.cache directory\n"
	@printf "\n"
	@printf "\033[1mDocker build targets:\033[0m\n"
	@printf "\033[31m\tdocker/build\033[39m   Compiles the FreiTest executable with Docker\n"
	@printf "\033[31m\tdocker/run\033[39m     Compiles and runs FreiTest with RUN_ARGS arguments inside the container\n"
	@printf "\033[31m\tdocker/package\033[39m Compiles and packages FreiTest as tar.gz archive\n"
	@printf "\033[31m\tdocker/image\033[39m   Compiles and packages FreiTest as tar.gz Docker image\n"
	@printf "\033[31m\tdocker/sysroot\033[39m Compiles and packages FreiTest as tar.gz archive with all libraries\n"
	@printf "\033[31m\tdocker/cluster\033[39m Compiles and packages FreiTest as tar.gz archive for the old UFR cluster\n"
	@printf "\n"

build:
	$(MAKE) local_build

run:
	$(MAKE) local_run

test:
	$(MAKE) local_test

docker/build:
	$(MAKE) docker_build

docker/run:
	$(MAKE) docker_run

docker/package:
	$(MAKE) docker_package \
		PACKAGE_NAME="freitest-$(shell date "+%Y-%m-%d_%H-%M-%S")-package.tar.gz"

docker/image:
	$(MAKE) docker_image \
		PACKAGE_NAME="freitest-$(shell date "+%Y-%m-%d_%H-%M-%S")-docker.tar.gz"

docker/sysroot:
	$(MAKE) docker_sysroot \
		PACKAGE_NAME="freitest-$(shell date "+%Y-%m-%d_%H-%M-%S")-sysroot.tar.gz"

docker/cluster:
	$(MAKE) docker_package \
		PACKAGE_NAME="freitest-$(shell date "+%Y-%m-%d_%H-%M-%S")-cluster.tar.gz" \
		BAZEL_BUILD_ARGS="--config=ivybridge" \
		DOCKER_TAG="cluster"

clean:
	$(BAZEL) clean --expunge --

mrproper: clean
	rm -rf ~/.cache/bazel/; true

local_build:
	$(BAZEL) build --config=${BUILD_CONFIG} ${BAZEL_BUILD_ARGS} ${BUILD_TARGET} -- ${BUILD_ARGS}

local_run:
	$(BAZEL) run --config=${BUILD_CONFIG} ${BAZEL_RUN_ARGS} ${BUILD_TARGET} -- ${RUN_ARGS}

local_test:
	$(BAZEL) test --config=${BUILD_CONFIG} ${BAZEL_TEST_ARGS} ${TEST_TARGET} $(addprefix --test_arg=,${TEST_ARGS}) --

docker_build:
	@printf "Building docker image with tag ${DOCKER_TAG}\n"
	docker build \
		--file ${DOCKER_FILE} \
		--target ${DOCKER_TARGET} \
		--tag freitest:${DOCKER_TAG} \
		--build-arg BUILD_TARGET="${BUILD_TARGET}" \
		--build-arg BUILD_CONFIG="${BUILD_CONFIG}" \
		--build-arg BUILD_ARGS="${BUILD_ARGS}" \
		--build-arg BAZEL_BUILD_ARGS="${BAZEL_BUILD_ARGS}" \
		--build-arg BAZEL_TEST_ARGS="${BAZEL_TEST_ARGS}" \
		--build-arg TEST_TARGET="${TEST_TARGET}" \
		--build-arg TEST_BUILD="${DOCKER_BUILD_TESTS}" \
		--build-arg TEST_RUN="${DOCKER_RUN_TESTS}" \
		--build-arg TEST_ARGS="$(addprefix --test_arg=,${TEST_ARGS})" \
		.

docker_run: docker_build
	@printf "Running the freitest:${DOCKER_TAG} docker image\n"
	@printf "\033[31m----> Warning: Only changes in the /workspace directory are permanent! <----\033[39m\n"
	docker run --rm -it --mount type=bind,source="$(CURDIR)",target=/workspace freitest:${DOCKER_TAG} \
		freitest ${RUN_ARGS}

docker_package: docker_build
	@printf "Exporting freitest:${DOCKER_TAG} docker as package\n"
	docker run --rm -a stdout -a stderr --mount type=bind,source="$(CURDIR)",target=/export freitest:${DOCKER_TAG} \
		bash -c 'tar -cf - /freitest | pigz -9 > /export/${PACKAGE_NAME}'

docker_image: docker_build
	@printf "Exporting freitest:${DOCKER_TAG} docker as image\n"
	docker save freitest:${DOCKER_TAG} | pigz -9 > ${PACKAGE_NAME}

docker_sysroot: docker_build
	@printf "Exporting freitest:${DOCKER_TAG} docker as sysroot\n"
	docker run --rm -a stdout -a stderr --mount type=bind,source="$(CURDIR)",target=/export freitest:${DOCKER_TAG} \
		bash -c 'tar -cf - $$(find / -maxdepth 1 -mindepth 1 -printf '\''/%f\n'\'' \
				| grep -v -E '\''boot|dev|export|init|lost\+found|media|mnt|proc|run|sys|tmp'\'') \
			| pigz -9 > /export/${PACKAGE_NAME}'

public:
	# Publishes FreiTest to public git repository
	mkdir -p public; true
	cd public && \
	    git clone git@github.com:TobiasFaller/FreiTest.git . \
			|| (git fetch --all && git rebase origin/main); \
		true
	python3 scripts/copy_files.py \
		--config public.txt --source . --destination public
	find public -type d -exec chmod 755 {} \;
	find public -type f -exec chmod 644 {} \;
	cd public; git add .
	cd public; git -c core.pager=cat diff --cached --summary

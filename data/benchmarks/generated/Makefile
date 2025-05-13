PROJECT_ROOT ?= ../../..
SCRIPT_PATH ?= ${PROJECT_ROOT}/scripts
GENERIC_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_meta.json

SEEDS_TINY_PATH ?= seeds_tiny.txt
SEEDS_SMALL_PATH ?= seeds_small.txt
SEEDS_MEDIUM_PATH ?= seeds_medium.txt
SEEDS_LARGE_PATH ?= seeds_large.txt
SEEDS_HUGE_PATH ?= seeds_huge.txt
OUTUPT_TINY_PATH ?= tiny
OUTUPT_SMALL_PATH ?= small
OUTUPT_MEDIUM_PATH ?= medium
OUTUPT_LARGE_PATH ?= large
OUTUPT_HUGE_PATH ?= huge

.PHONY: default help clean mrproper build tiny small medium large huge generate

default: build-generic

help:
	@echo "Generates random benchmarks"
	@echo "  default: Generate all random benchmarks for all cell-libraries"
	@echo "  build-generic: Generate all random benchmarks for generic cell-library"
	@echo "  tiny-generic: Generate tiny random benchmarks for generic cell-library"
	@echo "  small-generic: Generate small random benchmarks for generic cell-library"
	@echo "  medium-generic: Generate medium random benchmarks for generic cell-library"
	@echo "  large-generic: Generate large random benchmarks for generic cell-library"
	@echo "  huge-generic: Generate huge random benchmarks for generic cell-library"
	@echo ""
	@echo " To build for a 'custom' cell library use"
	@echo "  make build CELL_LIBRARY=.../custom_meta.json OUTPUT_SUFFIX=-custom"

clean:
	rm -rf ${OUTUPT_TINY_PATH} || true
	rm -rf ${OUTUPT_SMALL_PATH} || true
	rm -rf ${OUTUPT_MEDIUM_PATH} || true
	rm -rf ${OUTUPT_LARGE_PATH} || true
	rm -rf ${OUTUPT_HUGE_PATH} || true

mrproper: clean

%-generic:
	$(MAKE) $* \
		CELL_LIBRARY=${GENERIC_CELL_LIBRARY} \
		OUTPUT_SUFFIX=-generic

build:
	$(MAKE) tiny
	$(MAKE) small
	$(MAKE) medium
	$(MAKE) large
	$(MAKE) huge

tiny:
	$(MAKE) generate \
		CI=5 CO=3 CF=3 CG=15 VI=3 VO=1 VF=0 VG=3 \
		SEEDS_PATH=${SEEDS_TINY_PATH} OUTPUT_PATH=${OUTUPT_TINY_PATH}

small:
	$(MAKE) generate \
		CI=10 CO=5 CF=10 CG=50 VI=5 VO=2 VF=1 VG=10 \
		SEEDS_PATH=${SEEDS_SMALL_PATH} OUTPUT_PATH=${OUTUPT_SMALL_PATH}

medium:
	$(MAKE) generate \
		CI=25 CO=10 CF=25 CG=500 VI=10 VO=3 VF=2 VG=25 \
		SEEDS_PATH=${SEEDS_MEDIUM_PATH} OUTPUT_PATH=${OUTUPT_MEDIUM_PATH}

large:
	$(MAKE) generate \
		CI=50 CO=20 CF=250 CG=50000 VI=20 VO=10 VF=10 VG=500 \
		SEEDS_PATH=${SEEDS_LARGE_PATH} OUTPUT_PATH=${OUTUPT_LARGE_PATH}

huge:
	$(MAKE) generate \
		CI=50 CO=20 CF=1000 CG=500000 VI=20 VO=10 VF=10 VG=500 \
		SEEDS_PATH=${SEEDS_HUGE_PATH} OUTPUT_PATH=${OUTUPT_HUGE_PATH}

generate:
	mkdir -p ${OUTPUT_PATH}; true
	i=1; while read seed; do python3 ${SCRIPT_PATH}/generators/generate_benchmark.py \
		--seed $$seed \
		--library-path ${CELL_LIBRARY} \
		--circuit-path ${OUTPUT_PATH}/benchmark$$i${OUTPUT_SUFFIX}.v --circuit-name benchmark --circuit-inputs ${CI} --circuit-outputs ${CO} --circuit-flipflops ${CF} --circuit-gates ${CG} \
		--vcm-path ${OUTPUT_PATH}/benchmark$$i${OUTPUT_SUFFIX}-vcm.v --vcm-name vcm --vcm-inputs ${VI} --vcm-constraints ${VO} --vcm-flipflops ${VF} --vcm-gates ${VG} \
		--meta-path ${OUTPUT_PATH}/benchmark$$i${OUTPUT_SUFFIX}-meta.json; \
		i=$$(($$i + 1)); \
	done < ${SEEDS_PATH}

[Previous Topic](./IDESetup.md) - [Index](../../../README.md) -  [Next Topic](./CommonIssues.md)

# Build Setup

There are currently two ways to run FreiTest:
1. The first method is to use a docker container for development.
2. The second method is to build natively on the host syste
## Using Docker with VS Code or Manually

There are two ways to run FreiTest using docker:
1. **The first method is to use [Visual Studio Code and the Development Container Extension](2_IdeSetup.md#open-inside-development-container) (recommended)**
2. The second method is to build and use the container manually

### Docker via Visual Studio Code Development Container Extension

See [Visual Studio Code and the Development Container Extension](2_IdeSetup.md#open-inside-development-container) for instructions.

### Manually using Docker (not recommended)

Use the `make docker/build` command to quickly build the docker image from Dockerfile.ubuntu. The resulting image contains the FreiTest executable and a copy of the directories **data**, **scripts** and **settings**.
See below how to run FreiTest in the docker container.
To drop inside a **bash shell** in the dockerfile don't specify an executable (`docker run --rm -it --mount... freitest:latest`).
The mount arguments map the workspace directories into the docker container to provided data persistence for changes.

```bash
make docker/build

# Run the docker image with a command parameters
docker run --rm -it \
  --mount type=bind,source="$(pwd)"/data,target=/freitest/data \
  --mount type=bind,source="$(pwd)"/scripts,target=/freitest/scripts \
  --mount type=bind,source="$(pwd)"/settings,target=/freitest/settings \
  --mount type=bind,source="$(pwd)"/output,target=/freitest/output \
  freitest:latest ./freitest --Settings ..
```

## Using the Native Build

### Installation Native on Ubuntu 24.04 LTS

Install the dependencies as shown below:

```bash
curl -fsSL https://bazel.build/bazel-release.pub.gpg \
    | gpg --dearmor > /etc/apt/trusted.gpg.d/bazel.gpg \
  && echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" \
    | tee /etc/apt/sources.list.d/bazel.list \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    g++ gdb libboost-all-dev libgmp-dev libz-dev xz-utils libtbb-dev \
    python3 python3-libxml2 \
    make bazel unzip zip gnuplot pigz \
    git
```

## Installation Native on Gentoo

Install the dependencies as shown below:

```bash
# Install a Java runtime for Bazel
# emerge dev-java/icedtea-bin

# Add use static-libs in make.conf
emerge sys-devel/gdb dev-util/bazel \
  sys-libs/zlib  dev-libs/boost dev-libs/gmp dev-libs/mpfr dev-cpp/tbb \
  dev-python/matplotlib dev-python/numpy dev-python/lxml \
  sci-visualization/gnuplot app-arch/pigz
```

## Installation Native on CentOS / Alpine Linux

Install the dependencies as given in the Dockerfile.
No instructions will be provided here.

## Build and run with Bazel natively

Use the `make build`, `make run` and `make test` commands to build and run unit-tests of FreiTest.

```bash
make build
make run
make test
```

Bazel can be invoked manually via the commands below.
The switch `-s` can be used to log all executed commands and troubleshoot the build process.
Tests are written using the Boost test framework, the `--log_level=all` test option enables verbose output and the `--run_test=` test option filters executed tests.
See [Boost Test documentation](https://www.boost.org/doc/libs/1_73_0/libs/test/doc/html/boost_test/runtime_config.html) for more test options.

```bash
# Build and run FreiTest
bazel build //src:freitest

# Build troubleshooting with executed command output
bazel build -s //src:freitest

# Run the project with the specified paramters
bazel run --run_under="cd $(pwd) && " --config=debug //src:freitest -- \
  --Settings ..

# Run only one test in the test program VerilogSpiritParserTest
bazel test --config=debug \
    --test_output=all \
    --test_arg=--log_level=all \
    --test_arg=--run_test=VerilogSpiritParserTest/TestWithBasicFile \
    //test:VerilogSpiritParserTest

# Clean the workspace
bazel clean --expunge
```

There are currently three different configurations for Bazel which can be provided via the `--config` option.
These configurations are specified in the project root .bazelrc file.

- **release**: Relese build with all optimizations on
- **debug**: General purpose debugging configuration
- **asan**: Resource leak identification and in-depth troubleshooting
- **profiler**: Performance profiling

### Troubleshooting

When upgrading the compiler there can be the following problem.
Deleting the Bazel cache like shown below can help.

```text
ERROR: /.../freitest/src/BUILD:38:11: undeclared inclusion(s) in rule '//src:libfreitest':
this rule is missing dependency declarations for the following files included by 'src/TPG/LogicGenerator/Modules/InputLogic/ConstantInputLogicEncoder.cpp':
  '/usr/lib/gcc/x86_64-linux-gnu/10/include/stdint.h'
  '/usr/lib/gcc/x86_64-linux-gnu/10/include/stddef.h'
  '/usr/lib/gcc/x86_64-linux-gnu/10/include/stdarg.h'
  '/usr/lib/gcc/x86_64-linux-gnu/10/include/limits.h'
  '/usr/lib/gcc/x86_64-linux-gnu/10/include/syslimits.h'
```

It is also possible that the bazel command is not found after upgrading Ubuntu despite it beeing installed: `bazel: command not found`
Both problems can be fixed by deleting the cache.
If the problem still persist, try reinstalling Bazel.

```bash
rm -rf ~/.cache/bazel
```

[Previous Topic](./IDESetup.md) - [Index](../../../README.md) -  [Next Topic](./CommonIssues.md)

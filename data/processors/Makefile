PROJECT_ROOT ?= ../..
SCRIPTS_PATH ?= ${PROJECT_ROOT}/scripts
GENERIC_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_synthesis.lib

OUTPUT_DARKRISCV_PATH ?= darkriscv
OUTPUT_PICORV32_PATH ?= picorv32
SOURCES_DARKRISCV_PATH ?= .sources.darkriscv
SOURCES_PICORV32_PATH ?= .sources.picorv32

PARAMS_DARKRISCV_RV32E ?= params_darkriscv-rv32e.txt
PARAMS_DARKRISCV_RV32I ?= params_darkriscv-rv32i.txt
PARAMS_DARKRISCV_RV32I_ZICSR ?= params_darkriscv-rv32i-zicsr.txt
PARAMS_PICORV32_RV32E_ZICSR_ZICNTR ?= params_picorv32-rv32e-zicsr-zicntr.txt
PARAMS_PICORV32_RV32I_ZICSR_ZICNTR ?= params_picorv32-rv32i-zicsr-zicntr.txt
PARAMS_PICORV32_RV32IM_ZICSR_ZICNTR ?= params_picorv32-rv32im-zicsr-zicntr.txt
PARAMS_PICORV32_RV32IM_ZICSR_ZICNTR_XQREGS_XTIMER ?= params_picorv32-rv32im-zicsr-zicntr-xqregs-xtimer.txt

.PHONY: default help clean mrproper build darkriscv picorv32

default: build-generic

help:
	@echo "Generates processor benchmarks"
	@echo "  default: Generate all processor benchmarks for all cell-libraries"
	@echo "  build-generic: Generate all processor benchmarks for generic cell-library"
	@echo "  darkriscv-generic: Generate DarkRISCV benchmarks for generic cell-library"
	@echo "  picorv32-generic: Generate PicoRV32 benchmarks for generic cell-library"
	@echo ""
	@echo " To build for a 'custom' cell library use"
	@echo "  make build CELL_LIBRARY=.../custom_synthesis.lib OUTPUT_SUFFIX=-custom"

clean:
	rm -rf ${OUTPUT_DARKRISCV_PATH}; true
	rm -rf ${OUTPUT_PICORV32_PATH}; true

mrproper: clean
	rm -rf ${SOURCES_DARKRISCV_PATH}; true
	rm -rf ${SOURCES_PICORV32_PATH}; true

${SOURCES_DARKRISCV_PATH}/.extracted:
	mkdir -p ${SOURCES_DARKRISCV_PATH}
	git clone --depth=1 https://github.com/darklife/darkriscv ${SOURCES_DARKRISCV_PATH}; true
	cd ${SOURCES_DARKRISCV_PATH} && git reset --hard HEAD && git clean -dfx; true
	sed -i 's|$$stop|$$display|g' ${SOURCES_DARKRISCV_PATH}/rtl/darkriscv.v
	sed -i 's|`include "../rtl/config.vh"||g' ${SOURCES_DARKRISCV_PATH}/rtl/darkriscv.v

${SOURCES_PICORV32_PATH}/.extracted:
	mkdir -p ${SOURCES_PICORV32_PATH}
	git clone --depth=1 https://github.com/cliffordwolf/picorv32.git ${SOURCES_PICORV32_PATH}; true
	cd ${SOURCES_PICORV32_PATH} && git reset --hard HEAD && git clean -dfx; true

%-generic:
	$(MAKE) $* \
		CELL_LIBRARY=${GENERIC_CELL_LIBRARY} \
		OUTPUT_SUFFIX=-generic

build:
	$(MAKE) darkriscv
	$(MAKE) picorv32

darkriscv: ${SOURCES_DARKRISCV_PATH}/.extracted
	mkdir -p ${OUTPUT_DARKRISCV_PATH}
	$(MAKE) synthesize \
		TOP_MODULE=darkriscv \
		PARAMS_PATH=${PARAMS_DARKRISCV_RV32E} \
		SOURCE_PATH=${SOURCES_DARKRISCV_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_DARKRISCV_PATH}/darkriscv-rv32e${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_DARKRISCV_PATH}/darkriscv-rv32e${OUTPUT_SUFFIX}.v
	$(MAKE) synthesize \
		TOP_MODULE=darkriscv \
		PARAMS_PATH=${PARAMS_DARKRISCV_RV32I} \
		SOURCE_PATH=${SOURCES_DARKRISCV_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_DARKRISCV_PATH}/darkriscv-rv32i${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_DARKRISCV_PATH}/darkriscv-rv32i${OUTPUT_SUFFIX}.v
	$(MAKE) synthesize \
		TOP_MODULE=darkriscv \
		PARAMS_PATH=${PARAMS_DARKRISCV_RV32I_ZICSR} \
		SOURCE_PATH=${SOURCES_DARKRISCV_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_DARKRISCV_PATH}/darkriscv-rv32i-zicsr${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_DARKRISCV_PATH}/darkriscv-rv32i-zicsr${OUTPUT_SUFFIX}.v

picorv32: ${SOURCES_PICORV32_PATH}/.extracted
	mkdir -p ${OUTPUT_PICORV32_PATH}
	$(MAKE) synthesize \
		TOP_MODULE=picorv32 \
		PARAMS_PATH=${PARAMS_PICORV32_RV32E_ZICSR_ZICNTR} \
		SOURCE_PATH=${SOURCES_PICORV32_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_PICORV32_PATH}/picorv32-rv32e-zicsr-zicntr${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_PICORV32_PATH}/picorv32-rv32e-zicsr-zicntr${OUTPUT_SUFFIX}.v
	$(MAKE) synthesize \
		TOP_MODULE=picorv32 \
		PARAMS_PATH=${PARAMS_PICORV32_RV32I_ZICSR_ZICNTR} \
		SOURCE_PATH=${SOURCES_PICORV32_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_PICORV32_PATH}/picorv32-rv32i-zicsr-zicntr${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_PICORV32_PATH}/picorv32-rv32i-zicsr-zicntr${OUTPUT_SUFFIX}.v
	$(MAKE) synthesize \
		TOP_MODULE=picorv32 \
		PARAMS_PATH=${PARAMS_PICORV32_RV32IM_ZICSR_ZICNTR} \
		SOURCE_PATH=${SOURCES_PICORV32_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_PICORV32_PATH}/picorv32-rv32im-zicsr-zicntr${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_PICORV32_PATH}/picorv32-rv32im-zicsr-zicntr${OUTPUT_SUFFIX}.v
	$(MAKE) synthesize \
		TOP_MODULE=picorv32 \
		PARAMS_PATH=${PARAMS_PICORV32_RV32IM_ZICSR_ZICNTR_XQREGS_XTIMER} \
		SOURCE_PATH=${SOURCES_PICORV32_PATH}/picorv32.v \
		YOSYS_SCRIPT_PATH=${SOURCES_PICORV32_PATH}/picorv32-rv32im-zicsr-zicntr-xqregs-xtimer${OUTPUT_SUFFIX}.ys \
		OUTPUT_PATH=${OUTPUT_PICORV32_PATH}/picorv32-rv32im-zicsr-zicntr-xqregs-xtimer${OUTPUT_SUFFIX}.v

synthesize:
	python3 ${SCRIPTS_PATH}/generators/generate_yosys.py \
		--execute \
		--top-module ${TOP_MODULE} \
		--library-path ${CELL_LIBRARY} \
		--input-path ${SOURCE_PATH} \
		--parameter-path ${PARAMS_PATH} \
		--define-path ${PARAMS_PATH} \
		--output-script-path ${YOSYS_SCRIPT_PATH} \
		--output-circuit-path ${OUTPUT_PATH}


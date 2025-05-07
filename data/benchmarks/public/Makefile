PROJECT_ROOT ?= ../../..
GENERIC_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_synthesis.lib

SOURCES_ISCAS85_PATH ?= .sources.iscas85
SOURCES_ISCAS89_PATH ?= .sources.iscas89
SOURCES_ITC99_PATH ?= .sources.itc99
OUTPUT_ISCAS85_PATH ?= iscas85
OUTPUT_ISCAS89_PATH ?= iscas89
OUTPUT_ITC99_PATH ?= itc99

.PHONY: all help all-generic clean mrproper build itc99 iscas85 iscas89 convert_bench convert_blif

all: all-generic

help:
	@echo "Generates random benchmarks"
	@echo "  all: Generate random benchmarks for all cell-libraries"
	@echo "  all-generic: Generate random benchmarks for generic cell-library"
	@echo ""
	@echo " To build for a 'custom' cell library use"
	@echo "  make build CELL_LIBRARY=.../custom_synthesis.lib OUTPUT_SUFFIX=-custom"

all-generic:
	$(MAKE) build \
		CELL_LIBRARY=${GENERIC_CELL_LIBRARY} \
		OUTPUT_SUFFIX=-generic

clean:
	rm -rf ${OUTPUT_ISCAS85_PATH}; true
	rm -rf ${OUTPUT_ISCAS89_PATH}; true
	rm -rf ${OUTPUT_ITC99_PATH}; true

mrproper: clean
	rm -rf ${SOURCES_ISCAS85_PATH}; true
	rm -rf ${SOURCES_ISCAS89_PATH}; true
	rm -rf ${SOURCES_ITC99_PATH}; true

build:
	$(MAKE) iscas85
	$(MAKE) iscas89
	$(MAKE) itc99

${SOURCES_ISCAS85_PATH}:
	mkdir -p ${SOURCES_ISCAS85_PATH}
	wget -q --content-disposition -i sources_iscas85.txt -P ${SOURCES_ISCAS85_PATH}

${SOURCES_ISCAS89_PATH}:
	mkdir -p ${SOURCES_ISCAS89_PATH}
	wget --content-disposition -i sources_iscas89.txt -P ${SOURCES_ISCAS89_PATH}

${SOURCES_ITC99_PATH}:
	mkdir -p ${SOURCES_ITC99_PATH}
	git clone --depth=1 git@github.com:cad-polito-it/I99T.git ${SOURCES_ITC99_PATH}; true
	cd ${SOURCES_ITC99_PATH} && git reset --hard HEAD && git clean -dfx; true

iscas85: ${SOURCES_ISCAS85_PATH}
	mkdir -p ${OUTPUT_ISCAS85_PATH}; true
	for file in $$(find ${SOURCES_ISCAS85_PATH} -name '*.bench'); do \
		$(MAKE) convert_bench \
			SRC_BENCH=$$file \
			OUT_BLIF=${SOURCES_ISCAS85_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.blif; \
		$(MAKE) convert_blif \
			SRC_BLIF=${SOURCES_ISCAS85_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.blif \
			OUT_YOSYS=${SOURCES_ISCAS85_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.ys \
			OUT_VERILOG=${OUTPUT_ISCAS85_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.v; \
	done

iscas89: ${SOURCES_ISCAS89_PATH}
	mkdir -p ${OUTPUT_ISCAS89_PATH}; true
	for file in $$(find ${SOURCES_ISCAS89_PATH} -name '*.bench'); do \
		$(MAKE) convert_bench \
			SRC_BENCH=$$file \
			OUT_BLIF=${SOURCES_ISCAS89_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.blif; \
		$(MAKE) convert_blif \
			SRC_BLIF=${SOURCES_ISCAS89_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.blif \
			OUT_YOSYS=${SOURCES_ISCAS89_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.ys \
			OUT_VERILOG=${OUTPUT_ISCAS89_PATH}/$$(basename $$file .bench)${OUTPUT_SUFFIX}.v; \
	done

itc99: ${SOURCES_ITC99_PATH}
	mkdir -p ${OUTPUT_ITC99_PATH}; true
	for file in $$(find ${SOURCES_ITC99_PATH} -name '*_opt.bench'); do \
		$(MAKE) convert_bench \
			SRC_BENCH=$$file \
			OUT_BLIF=${SOURCES_ITC99_PATH}/$$(basename $$file _opt.bench)${OUTPUT_SUFFIX}.blif; \
		$(MAKE) convert_blif \
			SRC_BLIF=${SOURCES_ITC99_PATH}/$$(basename $$file _opt.bench)${OUTPUT_SUFFIX}.blif \
			OUT_YOSYS=${SOURCES_ITC99_PATH}/$$(basename $$file _opt.bench)${OUTPUT_SUFFIX}.ys \
			OUT_VERILOG=${OUTPUT_ITC99_PATH}/$$(basename $$file _opt.bench)${OUTPUT_SUFFIX}.v; \
	done

convert_bench:
	yosys-abc -q "read_bench ${SRC_BENCH}; write_blif ${OUT_BLIF}"
	# Clock ports have been removed in the bench format => reintroduce them
	if [ -n "$$(grep '^\.latch' ${OUT_BLIF})" ]; then \
		sed -E -i 's/^\.end/.inputs CLOCK\n.end/g' ${OUT_BLIF}; \
		sed -E -i 's/^\.latch(.*)(0|1|2|3)/.latch \1 re CLOCK \2/g' ${OUT_BLIF}; \
	fi

convert_blif:
	yosys -p "read_blif ${SRC_BLIF}; \
		rename -top benchmark; \
		techmap; \
		dfflibmap -liberty ${CELL_LIBRARY}; \
		abc -g AND,OR,XOR,NAND,NOR,XNOR,MUX -liberty ${CELL_LIBRARY}; \
		clean -purge; \
		write_verilog ${OUT_VERILOG}"

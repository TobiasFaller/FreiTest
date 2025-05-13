PROJECT_ROOT ?= ../../..
GENERIC_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_synthesis.lib

SOURCES_ISCAS85_PATH ?= .sources.iscas85
SOURCES_ISCAS89_PATH ?= .sources.iscas89
SOURCES_ITC99_PATH ?= .sources.itc99
SOURCES_IWS05_PATH ?= .sources.iws05
OUTPUT_ISCAS85_PATH ?= iscas85
OUTPUT_ISCAS89_PATH ?= iscas89
OUTPUT_ITC99_PATH ?= itc99
OUTPUT_IWS05_PATH ?= iws05

.PHONY: default help clean mrproper build itc99 iscas85 iscas89 iws05 convert_bench convert_blif

default: build-generic

help:
	@echo "Synthesizes public benchmarks"
	@echo "  default: Synthesize ISCAS, ITC (no IWS due to high runtime) benchmarks for all cell-libraries"
	@echo "  build-generic: Synthesize all public benchmarks for generic cell-library"
	@echo "  iscas85-generic: Synthesize iscas85 benchmarks for generic cell-library"
	@echo "  iscas89-generic: Synthesize iscas89 benchmarks for generic cell-library"
	@echo "  itc99-generic: Synthesize itc99 benchmarks for generic cell-library"
	@echo "  iws05-generic: Synthesize iws05 benchmarks for generic cell-library"
	@echo ""
	@echo " To build for a 'custom' cell library use"
	@echo "  make build CELL_LIBRARY=.../custom_synthesis.lib OUTPUT_SUFFIX=-custom"

clean:
	rm -rf ${OUTPUT_ISCAS85_PATH}; true
	rm -rf ${OUTPUT_ISCAS89_PATH}; true
	rm -rf ${OUTPUT_ITC99_PATH}; true
	rm -rf ${OUTPUT_IWS05_PATH}; true

mrproper: clean
	rm -rf ${SOURCES_ISCAS85_PATH}; true
	rm -rf ${SOURCES_ISCAS89_PATH}; true
	rm -rf ${SOURCES_ITC99_PATH}; true
	rm -rf ${SOURCES_IWS05_PATH}; true

build:
	$(MAKE) iscas85
	$(MAKE) iscas89
	$(MAKE) itc99

${SOURCES_ISCAS85_PATH}/.extracted:
	mkdir -p ${SOURCES_ISCAS85_PATH}
	wget -q --content-disposition -i sources_iscas85.txt -P ${SOURCES_ISCAS85_PATH}
	touch ${SOURCES_ISCAS85_PATH}/.extracted

${SOURCES_ISCAS89_PATH}/.extracted:
	mkdir -p ${SOURCES_ISCAS89_PATH}
	wget --content-disposition -i sources_iscas89.txt -P ${SOURCES_ISCAS89_PATH}
	touch ${SOURCES_ISCAS89_PATH}/.extracted

${SOURCES_ITC99_PATH}/.extracted:
	mkdir -p ${SOURCES_ITC99_PATH}
	git clone --depth=1 https://github.com/cad-polito-it/I99T.git ${SOURCES_ITC99_PATH}; true
	cd ${SOURCES_ITC99_PATH} && git reset --hard HEAD && git clean -dfx; true
	touch ${SOURCES_ITC99_PATH}/.extracted

${SOURCES_IWS05_PATH}/.extracted:
	mkdir -p ${SOURCES_IWS05_PATH}
	wget -O ${SOURCES_IWS05_PATH}/benchmarks.tar.gz https://iwls.org/iwls2005/IWLS_2005_benchmarks_V_1.0.tgz; true
	cd ${SOURCES_IWS05_PATH} && tar --strip-components=1 -xzf benchmarks.tar.gz; true
	touch ${SOURCES_IWS05_PATH}/.extracted

%-generic:
	$(MAKE) $* \
		CELL_LIBRARY=${GENERIC_CELL_LIBRARY} \
		OUTPUT_SUFFIX=-generic

iscas85: ${SOURCES_ISCAS85_PATH}/.extracted
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

iscas89: ${SOURCES_ISCAS89_PATH}/.extracted
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

itc99: ${SOURCES_ITC99_PATH}/.extracted
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

iws05: ${SOURCES_IWS05_PATH}/.extracted
	mkdir -p ${OUTPUT_IWS05_PATH}; true
	for file in $$(find ${SOURCES_IWS05_PATH} \
			-wholename '*/faraday/netlist/*.v' -or \
			-wholename '*/gaisler/netlist/*.v' -or \
			-wholename '*/opencores/netlist/*.v'); do \
		$(MAKE) convert_verilog \
			SRC_VERILOG=$$file \
			SRC_CELL_LIBRARY=${SOURCES_IWS05_PATH}/library/GSCLib_3.0.lib \
			OUT_VERILOG=${OUTPUT_IWS05_PATH}/$$(basename $$file .v)${OUTPUT_SUFFIX}.v; \
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

convert_verilog:
	yosys -p "read_liberty ${SRC_CELL_LIBRARY}; \
		read_verilog ${SRC_VERILOG}; \
		flatten; \
		hierarchy -auto-top; \
		async2sync; \
		techmap; \
		clean -purge; \
		dfflibmap -liberty ${CELL_LIBRARY}; \
		abc -g AND,OR,XOR,NAND,NOR,XNOR,MUX -liberty ${CELL_LIBRARY}; \
		clean -purge; \
		write_verilog ${OUT_VERILOG}"

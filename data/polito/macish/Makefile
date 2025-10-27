PROJECT_ROOT ?= $(realpath ../../..)
NANGATE45_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/nangate/nangate45_synthesis.lib
NANGATE45_CELL_VERILOG ?= ${PROJECT_ROOT}/data/cell-libraries/nangate/nangate45_functional.v
GENERIC_CELL_LIBRARY ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_synthesis.lib
GENERIC_CELL_VERILOG ?= ${PROJECT_ROOT}/data/cell-libraries/generic/generic_functional.v
SCRIPTS_PATH ?= ${PROJECT_ROOT}/scripts/generators

SOURCES_PATH ?= .
OUTPUT_PATH ?= output

.PHONY: default help clean mrproper build

default: build

help:
	@echo "Targets:"
	@echo "  macish-generic: Build macish sources for the generic cell library"
	@echo "  macish-nangate45: Build macish sources for the Nangate45 cell library"
	@echo "  macish-test: Test basic multipliers M0 to M4"
	@echo "  macish-stats: Test 8/16/32 bit multipliers and collect error statistics"
	@echo "  macish-atpg: Run safe fault analysis for multipliers with FreiTest"
	@echo "  macish-results-distribution: Plots the statistical error simulation results"
	@echo "  macish-results-classification: Plots correlation of maximum error limits and statistical error simulation results"
	@echo ""
	@echo " To build for a 'custom' cell library use"
	@echo "  make build CELL_LIBRARY=.../custom_synthesis.lib CELL_VERILOG=.../custom_functional.v OUTPUT_SUFFIX=-custom"

clean:
	rm ${OUTPUT_PATH} || true

mrproper: clean

%-generic:
	$(MAKE) $* \
		CELL_LIBRARY=${GENERIC_CELL_LIBRARY} \
		CELL_VERILOG=${GENERIC_CELL_VERILOG} \
		OUTPUT_SUFFIX=-generic

%-nangate45:
	$(MAKE) $* \
		CELL_LIBRARY=${NANGATE45_CELL_LIBRARY} \
		CELL_VERILOG=${NANGATE45_CELL_VERILOG} \
		OUTPUT_SUFFIX=-nangate45

build: | macish-generic

macish:
	mkdir -p ${OUTPUT_PATH}; true

	for impl in m0 m1 m2 m3 m4; do \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module $$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/mul2-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/mul2-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
	done

	for impl in accurate underd conven1 conven2 ish1 ish2; do \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MACISH8_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/macish8-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/macish8-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MUL8_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/mul8-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/mul8-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MACISH16_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/macish16-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/macish16-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MUL16_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/mul16-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/mul16-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MACISH32_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/macish32-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/macish32-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
		python3 ${SCRIPTS_PATH}/generate_yosys.py \
			--execute \
			--top-module MUL32_$$(echo $${impl} | tr '[:lower:]' '[:upper:]') \
			--input-path ${SOURCES_PATH}/macish.v \
			--output-circuit-path ${OUTPUT_PATH}/mul32-$${impl}${OUTPUT_SUFFIX}.v \
			--output-script-path ${OUTPUT_PATH}/mul32-$${impl}${OUTPUT_SUFFIX}.ys \
			--library-path ${CELL_LIBRARY} \
			--slang; \
	done

macish-test:
	verilator --Mdir ${OUTPUT_PATH}/tb-M0  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VM0_tb --top-module M0_tb -j 4 ${SOURCES_PATH}/macish-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-M1  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VM1_tb --top-module M1_tb -j 4 ${SOURCES_PATH}/macish-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-M2  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VM2_tb --top-module M2_tb -j 4 ${SOURCES_PATH}/macish-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-M3  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VM3_tb --top-module M3_tb -j 4 ${SOURCES_PATH}/macish-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-M4  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VM4_tb --top-module M4_tb -j 4 ${SOURCES_PATH}/macish-tb.v
	${OUTPUT_PATH}/tb-M0/VM0_tb
	${OUTPUT_PATH}/tb-M1/VM1_tb
	${OUTPUT_PATH}/tb-M2/VM2_tb
	${OUTPUT_PATH}/tb-M3/VM3_tb
	${OUTPUT_PATH}/tb-M4/VM4_tb

macish-stats:
	verilator --Mdir ${OUTPUT_PATH}/tb-MUL8  -DMUL8_TB  --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VMUL_tb --top-module MUL_tb -j 4 ${SOURCES_PATH}/macish-stats-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-MUL16 -DMUL16_TB --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VMUL_tb --top-module MUL_tb -j 4 ${SOURCES_PATH}/macish-stats-tb.v
	verilator --Mdir ${OUTPUT_PATH}/tb-MUL32 -DMUL32_TB --cc --main --exe --binary --timing --preproc-token-limit 1000000 --prefix VMUL_tb --top-module MUL_tb -j 4 ${SOURCES_PATH}/macish-stats-tb.v
	${OUTPUT_PATH}/tb-MUL8/VMUL_tb
	${OUTPUT_PATH}/tb-MUL16/VMUL_tb
	${OUTPUT_PATH}/tb-MUL32/VMUL_tb

macish-atpg:
	# Stuck-At
	for impl in underd conven1 conven2 ish1 ish2; do \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish8-mul-$${impl}-stuck-at.log \
			--Settings ./settings/circuits/polito/macish8-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-stuck-at.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish16-mul-$${impl}-stuck-at.log \
			--Settings ./settings/circuits/polito/macish16-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-stuck-at.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish32-mul-$${impl}-stuck-at.log \
			--Settings ./settings/circuits/polito/macish32-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-stuck-at.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
	done

	# Transition-Delay
	for impl in underd conven1 conven2 ish1 ish2; do \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish8-mul-$${impl}-transition.log \
			--Settings ./settings/circuits/polito/macish8-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-transition.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish16-mul-$${impl}-transition.log \
			--Settings ./settings/circuits/polito/macish16-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-transition.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish32-mul-$${impl}-transition.log \
			--Settings ./settings/circuits/polito/macish32-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-transition.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000; \
	done

	# Cell-Aware
	for impl in underd conven1 conven2 ish1 ish2; do \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish8-mul-$${impl}-cell-aware.log \
			--Settings ./settings/circuits/polito/macish8-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-cell-aware.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000 \
			--PoliTO/MacishSafeFaultAnalysis/UdfmImportPath ../data/model-ctm/out_udfm_iocopo_open_short_transistor/nangate45.functional.udfm; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish16-mul-$${impl}-cell-aware.log \
			--Settings ./settings/circuits/polito/macish16-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-cell-aware.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000 \
			--PoliTO/MacishSafeFaultAnalysis/UdfmImportPath ../data/model-ctm/out_udfm_iocopo_open_short_transistor/nangate45.functional.udfm; \
		cd ${PROJECT_ROOT} && ./bazel-bin/src/freitest --log-file=./output/macish32-mul-$${impl}-cell-aware.log \
			--Settings ./settings/circuits/polito/macish32-mul-$${impl}.freitest.jsonc \
			--Settings ./settings/workflows/polito/macish-safe-fault-analysis-cell-aware.freitest.jsonc \
			--PoliTO/MacishSafeFaultAnalysis/Complete Incomplete \
			--PoliTO/MacishSafeFaultAnalysis/MinDigits 8 \
			--PoliTO/MacishSafeFaultAnalysis/Samples 10000 \
			--PoliTO/MacishSafeFaultAnalysis/UdfmImportPath ../data/model-ctm/out_udfm_iocopo_open_short_transistor/nangate45.functional.udfm; \
	done

macish-results-distribution:
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL32_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2; do \
		echo "Analyzing $${name} Stuck-At Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_distribution.py \
			--name $${name}_STUCKAT --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisStuckAt/results.json \
			--output-path ${PROJECT_ROOT}/output; \
	done
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL32_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2; do \
		echo "Analyzing $${name} Transition-Delay Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_distribution.py \
			--name $${name}_TRANSITIONDELAY --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisTransitionDelay/results.json \
			--output-path ${PROJECT_ROOT}/output; \
	done
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL16_UNDERD MUL16_CONVEN1 MUL16_CONVEN2 MUL16_ISH1 MUL16_ISH2 \
			MUL32_ACCURATE MUL32_UNDERD MUL32_CONVEN1 MUL32_CONVEN2 MUL32_ISH1 MUL32_ISH2; do \
		echo "Analyzing $${name} Cell-Aware Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_distribution.py \
			--name $${name}_CELLAWARE --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisCellAware/results.json \
			--output-path ${PROJECT_ROOT}/output; \
	done

macish-results-classification:
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL32_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2; do \
		echo "Analyzing $${name} Stuck-At Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_plots.py \
			--name $${name}_STUCKAT --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisStuckAt/results.json \
			--output-path ${PROJECT_ROOT}/output --error-mean-max 0.0001 --error-limit-max 0.1; \
	done
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL32_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2; do \
		echo "Analyzing $${name} Transition-Delay Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_plots.py \
			--name $${name}_TRANSITIONDELAY --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisTransitionDelay/results.json \
			--output-path ${PROJECT_ROOT}/output --error-mean-max 0.0001 --error-limit-max 0.1; \
	done
	for name in MUL8_ACCURATE MUL8_UNDERD MUL8_CONVEN1 MUL8_CONVEN2 MUL8_ISH1 MUL8_ISH2 \
			MUL16_ACCURATE MUL16_UNDERD MUL16_CONVEN1 MUL16_CONVEN2 MUL16_ISH1 MUL16_ISH2 \
			MUL32_ACCURATE MUL32_UNDERD MUL32_CONVEN1 MUL32_CONVEN2 MUL32_ISH1 MUL32_ISH2; do \
		echo "Analyzing $${name} Cell-Aware Fault"; \
		python3 ${PROJECT_ROOT}/scripts/polito/macish_plots.py \
			--name $${name}_CELLAWARE --source-path ${PROJECT_ROOT}/output/$${name}_SafeFaultAnalysisCellAware/results.json \
			--output-path ${PROJECT_ROOT}/output --error-mean-max 0.0001 --error-limit-max 0.1; \
	done

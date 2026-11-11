PROJECT_ROOT ?= ../../..
NANGATE_LIBRARY ?= $(PROJECT_ROOT)/data/cell-libraries/nangate/nangate45_synthesis.lib
SCRIPTS_PATH ?= $(PROJECT_ROOT)/scripts/generators
VCM_OUTPUT_PATH ?= ./output

.PHONY: all clean

all: | clean build

help::
	@printf "The following targets are available:\n"
	@printf "\tclean: Clean outputs\n"
	@printf "\tbuild: Build all VCMs\n"
	@printf "\tmacish: Macish and Multiplier VCM\n"
	@printf "\trepeatable-constant-switching-ri5cy-rv32im-zicsr-xpulp2: RI5CY in RV32IM_Zicsr_Xpulp2 version\n"
	@printf "\tmultipoint-switching-ri5cy-rv32im-zicsr-xpulp2: RI5CY in RV32IM_Zicsr_Xpulp2 version\n"
	@printf "\tmultipoint-switching-ibex-rv32imc-zicsr: ibex in RV32IMC_Zicsr version \n"

build:: misc repeatable-constant-switching multipoint-switching

misc: macish

repeatable-constant-switching: repeatable-constant-switching-ri5cy-rv32im-zicsr-xpulp2

multipoint-switching: multipoint-switching-ri5cy-rv32im-zicsr-xpulp2 multipoint-switching-ibex-rv32imc-zicsr


clean:
	rm $(VCM_OUTPUT_PATH)/* || true

$(VCM_OUTPUT_PATH):
	mkdir -p $(VCM_OUTPUT_PATH)

macish: $(VCM_OUTPUT_PATH)
	mkdir -p ${OUTPUT_PATH}; true
	for bits in 8 16 32; do \
		python3 $(SCRIPTS_PATH)/generate_yosys.py \
			--execute \
			--top-module MACISH$${bits}_VCM \
			--input-path ./macish-vcm.sv \
			--output-circuit-path $(VCM_OUTPUT_PATH)/macish$${bits}-vcm-nangate45.v \
			--output-script-path $(VCM_OUTPUT_PATH)/macish$${bits}-vcm-nangate45.ys \
			--library-path $(NANGATE_LIBRARY); \
		python3 $(SCRIPTS_PATH)/generate_yosys.py \
			--execute \
			--top-module MUL$${bits}_VCM \
			--input-path ./macish-vcm.sv \
			--output-circuit-path $(VCM_OUTPUT_PATH)/macish$${bits}-mul-vcm-nangate45.v \
			--output-script-path $(VCM_OUTPUT_PATH)/macish$${bits}-mul-vcm-nangate45.ys \
			--library-path $(NANGATE_LIBRARY); \
	done

repeatable-constant-switching-ri5cy-rv32im-zicsr-xpulp2: $(VCM_OUTPUT_PATH)
	python3 $(SCRIPTS_PATH)/generate_yosys.py \
		--execute \
		--top-module ri5cy_vcm \
		--input-path ./repeatable-constant-switching-vcm-ri5cy.sv \
		--output-circuit-path $(VCM_OUTPUT_PATH)/repeatable-constant-switching-vcm-ri5cy-rv32im-zicsr-xpulp2-nangate.v \
		--output-script-path $(VCM_OUTPUT_PATH)/synth-repeatable-constant-switching-vcm-ri5cy-rv32im-zicsr-xpulp2.ys \
		--library-path $(NANGATE_LIBRARY)

multipoint-switching-ri5cy-rv32im-zicsr-xpulp2: $(VCM_OUTPUT_PATH)
	python3 $(SCRIPTS_PATH)/generate_yosys.py \
		--execute \
		--top-module ri5cy_vcm \
		--input-path ./multipoint-switching-vcm-ri5cy.sv \
		--output-circuit-path $(VCM_OUTPUT_PATH)/multipoint-switching-vcm-ri5cy-rv32im-zicsr-xpulp2-nangate.v \
		--output-script-path $(VCM_OUTPUT_PATH)/synth-multipoint-switching-vcm-ri5cy-rv32im-zicsr-xpulp2.ys \
		--library-path $(NANGATE_LIBRARY)

multipoint-switching-ibex-rv32imc-zicsr: $(VCM_OUTPUT_PATH)
	python3 $(SCRIPTS_PATH)/generate_yosys.py \
		--execute \
		--top-module ibex_vcm \
		--input-path ./multipoint-switching-vcm-ibex.sv \
		--output-circuit-path $(VCM_OUTPUT_PATH)/multipoint-switching-vcm-ibex-rv32imc-zicsr-nangate.v \
		--output-script-path $(VCM_OUTPUT_PATH)/synth-multipoint-switching-vcm-ibex-rv32imc-zicsr.ys \
		--library-path $(NANGATE_LIBRARY)


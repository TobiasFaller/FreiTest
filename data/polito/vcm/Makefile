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

build:: misc

misc: macish

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


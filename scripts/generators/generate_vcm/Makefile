RISCV_DBT_SOURCES := rv32i rv64i rv32i rv32m rv32a rv32f rv32d rv32c rv64i rv64m rv64a rv64f rv64d rv64c rvzicsr rvzifencei
RISCV_OPCODES_RATIFIED_SOURCES := rv32_c rv32_c_f rv32_i rv32_zbb rv32_zbkb rv32_zbs rv32_zk rv32_zkn rv32_zknd rv32_zkne rv32_zknh rv32_zks rv64_a rv64_c rv64_d rv64_f rv64_h rv64_i rv64_m rv64_q rv64_zba rv64_zbb rv64_zbkb rv64_zbs rv64_zcb rv64_zfh rv64_zk rv64_zkn rv64_zknd rv64_zkne rv64_zknh rv64_zks rv_a rv_c rv_c_d rv_d rv_d_zfh rv_f rv_h rv_i rv_m rv_q rv_q_zfh rv_s rv_svinval rv_system rv_v rv_v_aliases rv_zawrs rv_zba rv_zbb rv_zbc rv_zbkb rv_zbkc rv_zbkx rv_zbs rv_zcb rv_zcmp rv_zcmt rv_zfh rv_zicbo rv_zicsr rv_zifencei rv_zk rv_zkn rv_zknh rv_zks rv_zksed rv_zksh
RISCV_OPCODES_UNRATIFIED_SOURCES := rv32_d_zfa rv32_zbp rv32_zbpbo rv32_zbt rv32_zpn rv32_zpsf rv64_b rv64_q_zfa rv64_zbe rv64_zbf rv64_zbm rv64_zbp rv64_zbpbo rv64_zbr rv64_zbt rv64_zpn rv128_c rv128_i rv_b rv_c_zihintntl rv_d_zfa rv_f_zfa rv_q_zfa rv_smrnmi rv_zbe rv_zbf rv_zbp rv_zbpbo rv_zbr rv_zbt rv_zfh_zfa rv_zicond rv_zihintntl rv_zpn rv_zpsf
RISCV_CUSTOM_XPULP_SOURCES := xpulp_bit_manip xpulp_general_alu xpulp_hwloops xpulp_imm_branch xpulp_loads xpulp_mac xpulp_store xpulp_vectorial_compare xpulp_vectorial
RISCV_CUSTOM_PICORV_SOURCES := xpicorv_qregs xpicorv_timer

help:
	@printf "Validity Checker Module Targets:\n"
	@printf "\tconvert/all Prepares the JSON instruction specifications"
	@printf "\tconvert-dbt-rise/all Prepares the JSON instruction specifications from the dbt-rise repository"
	@printf "\tconvert-riscv-opcodes/all Prepares the JSON instruction specifications from the risc-v opcodes repository"
	@printf "\tconvert-custom/all Prepares the JSON instruction specifications from the custom specifications"

.PHONY: .FORCE
.FORCE:
.SECONDARY:
.SUFFIXES:

# Specification to instruction JSON conversion rules

convert/all: convert-dbt-rise/all convert-riscv-opcodes/all convert-custom/all
	@# This is a workaround since Make will ignore or fail this rule otherwise

convert-dbt-rise/all: $(addprefix convert-dbt-rise/, $(RISCV_DBT_SOURCES))
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-dbt-rise/rv%: json-dbt-rise/rv%.json
	@# This is a workaround since Make will ignore or fail this rule otherwise

convert-riscv-opcodes/all: $(addprefix convert-riscv-opcodes/, $(RISCV_OPCODES_RATIFIED_SOURCES) $(addprefix unratified/, $(RISCV_OPCODES_UNRATIFIED_SOURCES)))
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-riscv-opcodes/rv%: json-riscv-opcodes/rv%.json
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-riscv-opcodes/unratified/rv%: json-riscv-opcodes/unratified/rv%.json
	@# This is a workaround since Make will ignore or fail this rule otherwise

convert-custom/all:: convert-custom/xpulp convert-custom/xpicorv
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-custom/xpulp: $(addprefix convert-custom/, $(RISCV_CUSTOM_XPULP_SOURCES))
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-custom/xpulp%: json-custom/xpulp%.json
	@# This is a workaround since Make will ignore or fail this rule otherwise
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-custom/xpicorv: $(addprefix convert-custom/, $(RISCV_CUSTOM_PICORV_SOURCES))
	@# This is a workaround since Make will ignore or fail this rule otherwise
convert-custom/xpicorv%: json-custom/xpicorv%.json
	@# This is a workaround since Make will ignore or fail this rule otherwise



# -----------------------------------------------------------------------------
# Instruction to JSON conversion
# -----------------------------------------------------------------------------

# CoreDSL sources

json-dbt-rise/rv32i.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RV32I.core_desc \
		--isa RV32I \
		--exclude-instructions mret wfi

json-dbt-rise/rv64i.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RV64I.core_desc \
		--isa RV64I \
		--include-instructions lwu ld sd slli srli srai addiw slliw \
			srliw sraiw addw subw sllw srlw sraw

json-dbt-rise/rvzifencei.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RV32I.core_desc \
		--isa Zifencei \
		--include-instructions fence_i

json-dbt-rise/rvzicsr.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RV32I.core_desc \
		--isa Zicsr \
		--include-instructions csrrw csrrs csrrc csrrwi csrrsi csrrci

json-dbt-rise/rv32m.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVM.core_desc \
		--isa RV32M

json-dbt-rise/rv64m.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVM.core_desc \
		--isa RV64M

json-dbt-rise/rv32a.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVA.core_desc \
		--isa RV32A

json-dbt-rise/rv64a.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVA.core_desc \
		--isa RV64A

json-dbt-rise/rv32f.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVF.core_desc \
		--isa RV32F

json-dbt-rise/rv64f.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVF.core_desc \
		--isa RV64F

json-dbt-rise/rv32d.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVD.core_desc \
		--isa RV32D

json-dbt-rise/rv64d.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVD.core_desc \
		--isa RV64D

# Compressed instruction extension

# RV32:  Has compressed version of floating point instructions
#   - When F extension: c_flw c_fsw c_flwsp c_fswsp
#   - When FD extension: c_flw c_fsw c_flwsp c_fswsp c_fld c_fsd c_fldsp c_fsdsp
# RV64:  Has compressed version of floating point instructions
#   - When F extension: (none)
#   - When FD extension: c_fld c_fsd c_fldsp c_fsdsp
# RV128: Has no compressed version for floating point instructions

json-dbt-rise/rv32c.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV32IC \
		--exclude-instructions __reserved_clui __reserved_cmv dii

json-dbt-rise/rv32fc.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV32FC

json-dbt-rise/rv32dc.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV32DC

json-dbt-rise/rv64c.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV64IC

json-dbt-rise/rv64dc.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV32DC \
		--include-instructions cfld cfsd cfldsp cfsdsp

json-dbt-rise/rv128c.json: .FORCE
	python3 convert_core_desc_to_json.py \
		--output-file $@ \
		--core-descriptions dsl2-minres-riscv/RVC.core_desc \
		--isa RV128IC

# -----------------------------------------------------------------------------
# RISC-V Opcodes

json-riscv-opcodes/%.json: .FORCE
	python3 convert_riscv_desc_to_json.py \
		--output-file $@ \
		--riscv-opcodes $(basename $(@F))

json-riscv-opcodes/unratified/%.json: .FORCE
	python3 convert_riscv_desc_to_json.py \
		--output-file $@ \
		--riscv-opcodes unratified/$(basename $(@F))

# -----------------------------------------------------------------------------
# Custom extensions

json-custom/%.json: .FORCE
	python3 convert_custom_desc_to_json.py \
		--output-file $@ \
		--custom-descriptions custom-desc/$(basename $(@F)).txt


`default_nettype none

`include "isa/ri5cy-instructions.v"

module ri5cy_vcm(
	input logic \clk_i_g ,
	input logic \rst_ni_g ,

	// INSTRUCTION FETCH
	input logic \if_stage_i/perf_imiss_o_g ,
	input logic [31:0] \if_stage_i/pc_if_o_g ,
	input logic [31:0] \if_stage_i/pc_id_o_g ,

	// INSTRUCTION DECODE
	input logic [31:0] \id_stage_i/instr_rdata_i_g ,
	input logic [31:0] \id_stage_i/instr_rdata_i_gv ,
	input logic [4:0] \id_stage_i/controller_i/exc_cause_o_g ,

	// do_stall_signals: to be constrained at 000
	input logic \id_stage_i/controller_i/jr_stall_o_g ,
	input logic \id_stage_i/controller_i/load_stall_o_g ,

	// do_interrupt_signals to be constrained at 00
	input logic \cs_registers_i/m_irq_enable_o_g ,
	input logic \cs_registers_i/u_irq_enable_o_g ,

	// load_store_unit_i/ SIGNALS
	input logic \load_store_unit_i/data_misaligned_ex_i_g ,
	input logic \load_store_unit_i/data_err_i_g ,
	input logic [31:0] \load_store_unit_i/data_addr_o_g ,
	input logic \load_store_unit_i/ex_valid_i_g ,

	// 0: Adder | 1: Decoder | 2: Load Store Unit
	input logic [1:0] mode_of_operation_param,
	input logic vcm_init,

	output wire valid_adder_constraints_c,
	output wire valid_decoder_constraints_c,
	output wire valid_lsu_constraints_c
);

	wire clock = \clk_i_g ;

	wire if_miss = \if_stage_i/perf_imiss_o_g ;
	wire [31:0] if_pc = \if_stage_i/pc_if_o_g ;
	reg [31:0] if_pc_old; always_ff @(posedge clock) if_pc_old <= if_pc;

	wire [31:0] id_instruction = \id_stage_i/instr_rdata_i_g ;
	wire [31:0] id_instruction_valid = \id_stage_i/instr_rdata_i_gv ;
	wire [4:0] id_exc_cause = \id_stage_i/controller_i/exc_cause_o_g ;

	wire [1:0] do_stall_signals = {
		\id_stage_i/controller_i/jr_stall_o_g ,
		\id_stage_i/controller_i/load_stall_o_g
	};

	wire [1:0] do_interrupt_signals = {
		\cs_registers_i/m_irq_enable_o_g ,
		\cs_registers_i/u_irq_enable_o_g
	};

	wire ex_valid_i = \load_store_unit_i/ex_valid_i_g ;
	wire data_misaligned_ex_i = \load_store_unit_i/data_misaligned_ex_i_g ;
	wire data_err_i = \load_store_unit_i/data_err_i_g ;
	wire data_addr_o = \load_store_unit_i/data_addr_o_g ;

	wire is_lb_instruction;
	wire is_lh_instruction;
	wire is_lw_instruction;
	wire is_lbu_instruction;
	wire is_lhu_instruction;
	wire is_sb_instruction;
	wire is_sh_instruction;
	wire is_sw_instruction;
	wire is_p_lb_i_inc_instruction;
	wire is_p_lbu_i_inc_instruction;
	wire is_p_lh_i_inc_instruction;
	wire is_p_lhu_i_inc_instruction;
	wire is_p_lw_i_inc_instruction;
	wire is_p_lb_r_inc_instruction;
	wire is_p_lbu_r_inc_instruction;
	wire is_p_lh_r_inc_instruction;
	wire is_p_lhu_r_inc_instruction;
	wire is_p_lw_r_inc_instruction;
	wire is_p_lb_r_instruction;
	wire is_p_lbu_r_instruction;
	wire is_p_lh_r_instruction;
	wire is_p_lhu_r_instruction;
	wire is_p_lw_r_instruction;
	wire is_p_sb_i_inc_instruction;
	wire is_p_sh_i_inc_instruction;
	wire is_p_sw_i_inc_instruction;
	wire is_p_sb_r_inc_instruction;
	wire is_p_sh_r_inc_instruction;
	wire is_p_sw_r_inc_instruction;
	wire is_p_sb_r_instruction;
	wire is_p_sh_r_instruction;
	wire is_p_sw_r_instruction;
	wire is_add_instruction;
	wire is_addi_instruction;
	wire is_p_clip_instruction;
	wire is_p_abs_instruction;
	wire is_p_addn_instruction;
	wire is_p_addun_instruction;
	wire is_p_addrn_instruction;
	wire is_p_addurn_instruction;
	wire is_p_subn_instruction;
	wire is_p_subun_instruction;
	wire is_p_subrn_instruction;
	wire is_p_suburn_instruction;
	wire is_p_addnr_instruction;
	wire is_p_addunr_instruction;
	wire is_p_addrnr_instruction;
	wire is_p_addurnr_instruction;
	wire is_p_subnr_instruction;
	wire is_p_subunr_instruction;
	wire is_p_subrnr_instruction;
	wire is_p_suburnr_instruction;
	wire is_pv_add_h_instruction;
	wire is_pv_add_b_instruction;
	wire is_pv_sub_h_instruction;
	wire is_pv_sub_b_instruction;
	// NEW
	wire is_sub_instruction;
	wire is_slt_instruction;
	wire is_sltu_instruction;
	wire is_p_slet_instruction;
	wire is_p_sletu_instruction;
	wire is_p_min_instruction;
	wire is_p_minu_instruction;
	wire is_p_max_instruction;
	wire is_p_maxu_instruction;
	wire is_p_clipr_instruction;
	wire is_p_clipur_instruction;
	wire is_pv_add_sc_h_instruction;
	wire is_pv_add_sc_b_instruction;
	wire is_pv_sub_sc_h_instruction;
	wire is_pv_sub_sc_b_instruction;
	wire is_pv_avg_h_instruction;
	wire is_pv_avg_sc_h_instruction;
	wire is_pv_avg_b_instruction;
	wire is_pv_avg_sc_b_instruction;
	wire is_pv_avgu_h_instruction;
	wire is_pv_avgu_sc_h_instruction;
	wire is_pv_avgu_b_instruction;
	wire is_pv_avgu_sc_b_instruction;
	wire is_pv_min_h_instruction;
	wire is_pv_min_sc_h_instruction;
	wire is_pv_min_b_instruction;
	wire is_pv_min_sc_b_instruction;
	wire is_pv_minu_h_instruction;
	wire is_pv_minu_sc_h_instruction;
	wire is_pv_minu_b_instruction;
	wire is_pv_minu_sc_b_instruction;
	wire is_pv_max_h_instruction;
	wire is_pv_max_sc_h_instruction;
	wire is_pv_max_b_instruction;
	wire is_pv_max_sc_b_instruction;
	wire is_pv_maxu_h_instruction;
	wire is_pv_maxu_sc_h_instruction;
	wire is_pv_maxu_b_instruction;
	wire is_pv_maxu_sc_b_instruction;
	wire is_pv_dotup_h_instruction;
	wire is_pv_dotup_sc_h_instruction;
	wire is_pv_dotup_b_instruction;
	wire is_pv_dotup_sc_b_instruction;
	wire is_pv_dotusp_h_instruction;
	wire is_pv_dotusp_sc_h_instruction;
	wire is_pv_dotusp_b_instruction;
	wire is_pv_dotusp_sc_b_instruction;
	wire is_pv_dotsp_h_instruction;
	wire is_pv_dotsp_sc_h_instruction;
	wire is_pv_dotsp_b_instruction;
	wire is_pv_dotsp_sc_b_instruction;
	wire is_pv_sdotup_h_instruction;
	wire is_pv_sdotup_sc_h_instruction;
	wire is_pv_sdotup_b_instruction;
	wire is_pv_sdotup_sc_b_instruction;
	wire is_pv_sdotusp_h_instruction;
	wire is_pv_sdotusp_sc_h_instruction;
	wire is_pv_sdotusp_b_instruction;
	wire is_pv_sdotusp_sc_b_instruction;
	wire is_pv_sdotsp_h_instruction;
	wire is_pv_sdotsp_sc_h_instruction;
	wire is_pv_sdotsp_b_instruction;
	wire is_pv_sdotsp_sc_b_instruction;
	wire is_pv_shuffle_h_instruction;
	wire is_pv_shuffle_b_instruction;
	wire is_pv_shuffle2_h_instruction;
	wire is_pv_shuffle2_b_instruction;
	wire is_pv_cmpeq_h_instruction;
	wire is_pv_cmpeq_sc_h_instruction;
	wire is_pv_cmpeq_b_instruction;
	wire is_pv_cmpeq_sc_b_instruction;
	wire is_pv_cmpne_h_instruction;
	wire is_pv_cmpne_sc_h_instruction;
	wire is_pv_cmpne_b_instruction;
	wire is_pv_cmpne_sc_b_instruction;
	wire is_pv_cmpgt_h_instruction;
	wire is_pv_cmpgt_sc_h_instruction;
	wire is_pv_cmpgt_b_instruction;
	wire is_pv_cmpgt_sc_b_instruction;
	wire is_pv_cmpge_h_instruction;
	wire is_pv_cmpge_sc_h_instruction;
	wire is_pv_cmpge_b_instruction;
	wire is_pv_cmpge_sc_b_instruction;
	wire is_pv_cmplt_h_instruction;
	wire is_pv_cmplt_sc_h_instruction;
	wire is_pv_cmplt_b_instruction;
	wire is_pv_cmplt_sc_b_instruction;
	wire is_pv_cmple_h_instruction;
	wire is_pv_cmple_sc_h_instruction;
	wire is_pv_cmple_b_instruction;
	wire is_pv_cmple_sc_b_instruction;
	wire is_pv_cmpgtu_h_instruction;
	wire is_pv_cmpgtu_sc_h_instruction;
	wire is_pv_cmpgtu_b_instruction;
	wire is_pv_cmpgtu_sc_b_instruction;
	wire is_pv_cmpgeu_h_instruction;
	wire is_pv_cmpgeu_sc_h_instruction;
	wire is_pv_cmpgeu_b_instruction;
	wire is_pv_cmpgeu_sc_b_instruction;
	wire is_pv_cmpltu_h_instruction;
	wire is_pv_cmpltu_sc_h_instruction;
	wire is_pv_cmpltu_b_instruction;
	wire is_pv_cmpltu_sc_b_instruction;
	wire is_pv_cmpleu_h_instruction;
	wire is_pv_cmpleu_sc_h_instruction;
	wire is_pv_cmpleu_b_instruction;
	wire is_pv_cmpleu_sc_b_instruction;
	wire is_slti_instruction;
	wire is_sltiu_instruction;
	wire is_p_machhsn_instruction;
	wire is_p_macsrn_instruction;
	wire is_p_machhsrn_instruction;
	wire is_p_macun_instruction;
	wire is_p_machhun_instruction;
	wire is_p_macurn_instruction;
	wire is_p_machhurn_instruction;
	wire is_p_macsn_instruction;
	wire is_p_clipu_instruction;
	wire is_pv_add_sci_h_instruction;
	wire is_pv_add_sci_b_instruction;
	wire is_pv_sub_sci_h_instruction;
	wire is_pv_sub_sci_b_instruction;
	wire is_pv_avg_sci_h_instruction;
	wire is_pv_avg_sci_b_instruction;
	wire is_pv_min_sci_h_instruction;
	wire is_pv_min_sci_b_instruction;
	wire is_pv_max_sci_h_instruction;
	wire is_pv_max_sci_b_instruction;
	wire is_pv_dotusp_sci_h_instruction;
	wire is_pv_dotusp_sci_b_instruction;
	wire is_pv_dotsp_sci_h_instruction;
	wire is_pv_dotsp_sci_b_instruction;
	wire is_pv_sdotusp_sci_h_instruction;
	wire is_pv_sdotusp_sci_b_instruction;
	wire is_pv_sdotsp_sci_h_instruction;
	wire is_pv_sdotsp_sci_b_instruction;
	wire is_pv_cmple_sci_b_instruction;
	wire is_pv_cmple_sci_h_instruction;
	wire is_pv_cmplt_sci_b_instruction;
	wire is_pv_cmplt_sci_h_instruction;
	wire is_pv_cmpge_sci_b_instruction;
	wire is_pv_cmpge_sci_h_instruction;
	wire is_pv_cmpgt_sci_b_instruction;
	wire is_pv_cmpgt_sci_h_instruction;
	wire is_pv_cmpne_sci_b_instruction;
	wire is_pv_cmpne_sci_h_instruction;
	wire is_pv_cmpeq_sci_b_instruction;
	wire is_pv_cmpeq_sci_h_instruction;
	wire is_pv_shuffle_sci_h_instruction;
	wire is_pv_shufflei0_sci_b_instruction;
	wire is_pv_shufflei1_sci_b_instruction;
	wire is_pv_shufflei2_sci_b_instruction;
	wire is_pv_shufflei3_sci_b_instruction;
	wire is_pv_avgu_sci_h_instruction;
	wire is_pv_avgu_sci_b_instruction;
	wire is_pv_minu_sci_h_instruction;
	wire is_pv_minu_sci_b_instruction;
	wire is_pv_maxu_sci_h_instruction;
	wire is_pv_maxu_sci_b_instruction;
	wire is_pv_dotup_sci_h_instruction;
	wire is_pv_dotup_sci_b_instruction;
	wire is_pv_sdotup_sci_h_instruction;
	wire is_pv_sdotup_sci_b_instruction;
	wire is_pv_cmpleu_sci_b_instruction;
	wire is_pv_cmpleu_sci_h_instruction;
	wire is_pv_cmpltu_sci_b_instruction;
	wire is_pv_cmpltu_sci_h_instruction;
	wire is_pv_cmpgeu_sci_b_instruction;
	wire is_pv_cmpgeu_sci_h_instruction;
	wire is_pv_cmpgtu_sci_b_instruction;
	wire is_pv_cmpgtu_sci_h_instruction;
	wire is_pv_abs_h_instruction;
	wire is_pv_abs_b_instruction;
	wire is_jal_instruction;
	wire is_jalr_instruction;
	wire is_beq_instruction;
	wire is_bne_instruction;
	wire is_blt_instruction;
	wire is_bge_instruction;
	wire is_bltu_instruction;
	wire is_bgeu_instruction;

	wire has_operand_rs1;
	wire has_operand_rs2;
	wire has_operand_rd;
	wire has_operand_csr;
	wire [4:0] operand_rs1;
	wire [4:0] operand_rs2;
	wire [4:0] operand_rd;
	wire [11:0] operand_csr;

	wire [11:0] operand_imm_addi;
	wire [11:0] operand_imm_p_lb_i_inc;
	wire [11:0] operand_imm_p_lbu_i_inc;
	wire [11:0] operand_imm_p_lh_i_inc;
	wire [11:0] operand_imm_p_lhu_i_inc;
	wire [11:0] operand_imm_p_lw_i_inc;
	wire [11:0] operand_imm_p_sb_i_inc;
	wire [11:0] operand_imm_p_sh_i_inc;
	wire [11:0] operand_imm_p_sw_i_inc;

	wire opcode_valid;
	rv32im_zicsr_xpulp2_ri5cy rv_inst(
		.opcode(id_instruction),
		.opcode_valid(opcode_valid),
		.is_sub_instruction(is_sub_instruction),
		.is_slt_instruction(is_slt_instruction),
		.is_sltu_instruction(is_sltu_instruction),
		.is_p_slet_instruction(is_p_slet_instruction),
		.is_p_sletu_instruction(is_p_sletu_instruction),
		.is_p_min_instruction(is_p_min_instruction),
		.is_p_minu_instruction(is_p_minu_instruction),
		.is_p_max_instruction(is_p_max_instruction),
		.is_p_maxu_instruction(is_p_maxu_instruction),
		.is_p_clipr_instruction(is_p_clipr_instruction),
		.is_p_clipur_instruction(is_p_clipur_instruction),
		.is_pv_add_sc_h_instruction(is_pv_add_sc_h_instruction),
		.is_pv_add_sc_b_instruction(is_pv_add_sc_b_instruction),
		.is_pv_sub_sc_h_instruction(is_pv_sub_sc_h_instruction),
		.is_pv_sub_sc_b_instruction(is_pv_sub_sc_b_instruction),
		.is_pv_avg_h_instruction(is_pv_avg_h_instruction),
		.is_pv_avg_sc_h_instruction(is_pv_avg_sc_h_instruction),
		.is_pv_avg_b_instruction(is_pv_avg_b_instruction),
		.is_pv_avg_sc_b_instruction(is_pv_avg_sc_b_instruction),
		.is_pv_avgu_h_instruction(is_pv_avgu_h_instruction),
		.is_pv_avgu_sc_h_instruction(is_pv_avgu_sc_h_instruction),
		.is_pv_avgu_b_instruction(is_pv_avgu_b_instruction),
		.is_pv_avgu_sc_b_instruction(is_pv_avgu_sc_b_instruction),
		.is_pv_min_h_instruction(is_pv_min_h_instruction),
		.is_pv_min_sc_h_instruction(is_pv_min_sc_h_instruction),
		.is_pv_min_b_instruction(is_pv_min_b_instruction),
		.is_pv_min_sc_b_instruction(is_pv_min_sc_b_instruction),
		.is_pv_minu_h_instruction(is_pv_minu_h_instruction),
		.is_pv_minu_sc_h_instruction(is_pv_minu_sc_h_instruction),
		.is_pv_minu_b_instruction(is_pv_minu_b_instruction),
		.is_pv_minu_sc_b_instruction(is_pv_minu_sc_b_instruction),
		.is_pv_max_h_instruction(is_pv_max_h_instruction),
		.is_pv_max_sc_h_instruction(is_pv_max_sc_h_instruction),
		.is_pv_max_b_instruction(is_pv_max_b_instruction),
		.is_pv_max_sc_b_instruction(is_pv_max_sc_b_instruction),
		.is_pv_maxu_h_instruction(is_pv_maxu_h_instruction),
		.is_pv_maxu_sc_h_instruction(is_pv_maxu_sc_h_instruction),
		.is_pv_maxu_b_instruction(is_pv_maxu_b_instruction),
		.is_pv_maxu_sc_b_instruction(is_pv_maxu_sc_b_instruction),
		.is_pv_dotup_h_instruction(is_pv_dotup_h_instruction),
		.is_pv_dotup_sc_h_instruction(is_pv_dotup_sc_h_instruction),
		.is_pv_dotup_b_instruction(is_pv_dotup_b_instruction),
		.is_pv_dotup_sc_b_instruction(is_pv_dotup_sc_b_instruction),
		.is_pv_dotusp_h_instruction(is_pv_dotusp_h_instruction),
		.is_pv_dotusp_sc_h_instruction(is_pv_dotusp_sc_h_instruction),
		.is_pv_dotusp_b_instruction(is_pv_dotusp_b_instruction),
		.is_pv_dotusp_sc_b_instruction(is_pv_dotusp_sc_b_instruction),
		.is_pv_dotsp_h_instruction(is_pv_dotsp_h_instruction),
		.is_pv_dotsp_sc_h_instruction(is_pv_dotsp_sc_h_instruction),
		.is_pv_dotsp_b_instruction(is_pv_dotsp_b_instruction),
		.is_pv_dotsp_sc_b_instruction(is_pv_dotsp_sc_b_instruction),
		.is_pv_sdotup_h_instruction(is_pv_sdotup_h_instruction),
		.is_pv_sdotup_sc_h_instruction(is_pv_sdotup_sc_h_instruction),
		.is_pv_sdotup_b_instruction(is_pv_sdotup_b_instruction),
		.is_pv_sdotup_sc_b_instruction(is_pv_sdotup_sc_b_instruction),
		.is_pv_sdotusp_h_instruction(is_pv_sdotusp_h_instruction),
		.is_pv_sdotusp_sc_h_instruction(is_pv_sdotusp_sc_h_instruction),
		.is_pv_sdotusp_b_instruction(is_pv_sdotusp_b_instruction),
		.is_pv_sdotusp_sc_b_instruction(is_pv_sdotusp_sc_b_instruction),
		.is_pv_sdotsp_h_instruction(is_pv_sdotsp_h_instruction),
		.is_pv_sdotsp_sc_h_instruction(is_pv_sdotsp_sc_h_instruction),
		.is_pv_sdotsp_b_instruction(is_pv_sdotsp_b_instruction),
		.is_pv_sdotsp_sc_b_instruction(is_pv_sdotsp_sc_b_instruction),
		.is_pv_shuffle_h_instruction(is_pv_shuffle_h_instruction),
		.is_pv_shuffle_b_instruction(is_pv_shuffle_b_instruction),
		.is_pv_shuffle2_h_instruction(is_pv_shuffle2_h_instruction),
		.is_pv_shuffle2_b_instruction(is_pv_shuffle2_b_instruction),
		.is_pv_cmpeq_h_instruction(is_pv_cmpeq_h_instruction),
		.is_pv_cmpeq_sc_h_instruction(is_pv_cmpeq_sc_h_instruction),
		.is_pv_cmpeq_b_instruction(is_pv_cmpeq_b_instruction),
		.is_pv_cmpeq_sc_b_instruction(is_pv_cmpeq_sc_b_instruction),
		.is_pv_cmpne_h_instruction(is_pv_cmpne_h_instruction),
		.is_pv_cmpne_sc_h_instruction(is_pv_cmpne_sc_h_instruction),
		.is_pv_cmpne_b_instruction(is_pv_cmpne_b_instruction),
		.is_pv_cmpne_sc_b_instruction(is_pv_cmpne_sc_b_instruction),
		.is_pv_cmpgt_h_instruction(is_pv_cmpgt_h_instruction),
		.is_pv_cmpgt_sc_h_instruction(is_pv_cmpgt_sc_h_instruction),
		.is_pv_cmpgt_b_instruction(is_pv_cmpgt_b_instruction),
		.is_pv_cmpgt_sc_b_instruction(is_pv_cmpgt_sc_b_instruction),
		.is_pv_cmpge_h_instruction(is_pv_cmpge_h_instruction),
		.is_pv_cmpge_sc_h_instruction(is_pv_cmpge_sc_h_instruction),
		.is_pv_cmpge_b_instruction(is_pv_cmpge_b_instruction),
		.is_pv_cmpge_sc_b_instruction(is_pv_cmpge_sc_b_instruction),
		.is_pv_cmplt_h_instruction(is_pv_cmplt_h_instruction),
		.is_pv_cmplt_sc_h_instruction(is_pv_cmplt_sc_h_instruction),
		.is_pv_cmplt_b_instruction(is_pv_cmplt_b_instruction),
		.is_pv_cmplt_sc_b_instruction(is_pv_cmplt_sc_b_instruction),
		.is_pv_cmple_h_instruction(is_pv_cmple_h_instruction),
		.is_pv_cmple_sc_h_instruction(is_pv_cmple_sc_h_instruction),
		.is_pv_cmple_b_instruction(is_pv_cmple_b_instruction),
		.is_pv_cmple_sc_b_instruction(is_pv_cmple_sc_b_instruction),
		.is_pv_cmpgtu_h_instruction(is_pv_cmpgtu_h_instruction),
		.is_pv_cmpgtu_sc_h_instruction(is_pv_cmpgtu_sc_h_instruction),
		.is_pv_cmpgtu_b_instruction(is_pv_cmpgtu_b_instruction),
		.is_pv_cmpgtu_sc_b_instruction(is_pv_cmpgtu_sc_b_instruction),
		.is_pv_cmpgeu_h_instruction(is_pv_cmpgeu_h_instruction),
		.is_pv_cmpgeu_sc_h_instruction(is_pv_cmpgeu_sc_h_instruction),
		.is_pv_cmpgeu_b_instruction(is_pv_cmpgeu_b_instruction),
		.is_pv_cmpgeu_sc_b_instruction(is_pv_cmpgeu_sc_b_instruction),
		.is_pv_cmpltu_h_instruction(is_pv_cmpltu_h_instruction),
		.is_pv_cmpltu_sc_h_instruction(is_pv_cmpltu_sc_h_instruction),
		.is_pv_cmpltu_b_instruction(is_pv_cmpltu_b_instruction),
		.is_pv_cmpltu_sc_b_instruction(is_pv_cmpltu_sc_b_instruction),
		.is_pv_cmpleu_h_instruction(is_pv_cmpleu_h_instruction),
		.is_pv_cmpleu_sc_h_instruction(is_pv_cmpleu_sc_h_instruction),
		.is_pv_cmpleu_b_instruction(is_pv_cmpleu_b_instruction),
		.is_pv_cmpleu_sc_b_instruction(is_pv_cmpleu_sc_b_instruction),
		.is_slti_instruction(is_slti_instruction),
		.is_sltiu_instruction(is_sltiu_instruction),
		.is_p_machhsn_instruction(is_p_machhsn_instruction),
		.is_p_macsrn_instruction(is_p_macsrn_instruction),
		.is_p_machhsrn_instruction(is_p_machhsrn_instruction),
		.is_p_macun_instruction(is_p_macun_instruction),
		.is_p_machhun_instruction(is_p_machhun_instruction),
		.is_p_macurn_instruction(is_p_macurn_instruction),
		.is_p_machhurn_instruction(is_p_machhurn_instruction),
		.is_p_macsn_instruction(is_p_macsn_instruction),
		.is_p_clipu_instruction(is_p_clipu_instruction),
		.is_pv_add_sci_h_instruction(is_pv_add_sci_h_instruction),
		.is_pv_add_sci_b_instruction(is_pv_add_sci_b_instruction),
		.is_pv_sub_sci_h_instruction(is_pv_sub_sci_h_instruction),
		.is_pv_sub_sci_b_instruction(is_pv_sub_sci_b_instruction),
		.is_pv_avg_sci_h_instruction(is_pv_avg_sci_h_instruction),
		.is_pv_avg_sci_b_instruction(is_pv_avg_sci_b_instruction),
		.is_pv_min_sci_h_instruction(is_pv_min_sci_h_instruction),
		.is_pv_min_sci_b_instruction(is_pv_min_sci_b_instruction),
		.is_pv_max_sci_h_instruction(is_pv_max_sci_h_instruction),
		.is_pv_max_sci_b_instruction(is_pv_max_sci_b_instruction),
		.is_pv_dotusp_sci_h_instruction(is_pv_dotusp_sci_h_instruction),
		.is_pv_dotusp_sci_b_instruction(is_pv_dotusp_sci_b_instruction),
		.is_pv_dotsp_sci_h_instruction(is_pv_dotsp_sci_h_instruction),
		.is_pv_dotsp_sci_b_instruction(is_pv_dotsp_sci_b_instruction),
		.is_pv_sdotusp_sci_h_instruction(is_pv_sdotusp_sci_h_instruction),
		.is_pv_sdotusp_sci_b_instruction(is_pv_sdotusp_sci_b_instruction),
		.is_pv_sdotsp_sci_h_instruction(is_pv_sdotsp_sci_h_instruction),
		.is_pv_sdotsp_sci_b_instruction(is_pv_sdotsp_sci_b_instruction),
		.is_pv_cmple_sci_b_instruction(is_pv_cmple_sci_b_instruction),
		.is_pv_cmple_sci_h_instruction(is_pv_cmple_sci_h_instruction),
		.is_pv_cmplt_sci_b_instruction(is_pv_cmplt_sci_b_instruction),
		.is_pv_cmplt_sci_h_instruction(is_pv_cmplt_sci_h_instruction),
		.is_pv_cmpge_sci_b_instruction(is_pv_cmpge_sci_b_instruction),
		.is_pv_cmpge_sci_h_instruction(is_pv_cmpge_sci_h_instruction),
		.is_pv_cmpgt_sci_b_instruction(is_pv_cmpgt_sci_b_instruction),
		.is_pv_cmpgt_sci_h_instruction(is_pv_cmpgt_sci_h_instruction),
		.is_pv_cmpne_sci_b_instruction(is_pv_cmpne_sci_b_instruction),
		.is_pv_cmpne_sci_h_instruction(is_pv_cmpne_sci_h_instruction),
		.is_pv_cmpeq_sci_b_instruction(is_pv_cmpeq_sci_b_instruction),
		.is_pv_cmpeq_sci_h_instruction(is_pv_cmpeq_sci_h_instruction),
		.is_pv_shuffle_sci_h_instruction(is_pv_shuffle_sci_h_instruction),
		.is_pv_shufflei0_sci_b_instruction(is_pv_shufflei0_sci_b_instruction),
		.is_pv_shufflei1_sci_b_instruction(is_pv_shufflei1_sci_b_instruction),
		.is_pv_shufflei2_sci_b_instruction(is_pv_shufflei2_sci_b_instruction),
		.is_pv_shufflei3_sci_b_instruction(is_pv_shufflei3_sci_b_instruction),
		.is_pv_avgu_sci_h_instruction(is_pv_avgu_sci_h_instruction),
		.is_pv_avgu_sci_b_instruction(is_pv_avgu_sci_b_instruction),
		.is_pv_minu_sci_h_instruction(is_pv_minu_sci_h_instruction),
		.is_pv_minu_sci_b_instruction(is_pv_minu_sci_b_instruction),
		.is_pv_maxu_sci_h_instruction(is_pv_maxu_sci_h_instruction),
		.is_pv_maxu_sci_b_instruction(is_pv_maxu_sci_b_instruction),
		.is_pv_dotup_sci_h_instruction(is_pv_dotup_sci_h_instruction),
		.is_pv_dotup_sci_b_instruction(is_pv_dotup_sci_b_instruction),
		.is_pv_sdotup_sci_h_instruction(is_pv_sdotup_sci_h_instruction),
		.is_pv_sdotup_sci_b_instruction(is_pv_sdotup_sci_b_instruction),
		.is_pv_cmpleu_sci_b_instruction(is_pv_cmpleu_sci_b_instruction),
		.is_pv_cmpleu_sci_h_instruction(is_pv_cmpleu_sci_h_instruction),
		.is_pv_cmpltu_sci_b_instruction(is_pv_cmpltu_sci_b_instruction),
		.is_pv_cmpltu_sci_h_instruction(is_pv_cmpltu_sci_h_instruction),
		.is_pv_cmpgeu_sci_b_instruction(is_pv_cmpgeu_sci_b_instruction),
		.is_pv_cmpgeu_sci_h_instruction(is_pv_cmpgeu_sci_h_instruction),
		.is_pv_cmpgtu_sci_b_instruction(is_pv_cmpgtu_sci_b_instruction),
		.is_pv_cmpgtu_sci_h_instruction(is_pv_cmpgtu_sci_h_instruction),
		.is_pv_abs_h_instruction(is_pv_abs_h_instruction),
		.is_pv_abs_b_instruction(is_pv_abs_b_instruction),
		.is_lb_instruction(is_lb_instruction),
		.is_lh_instruction(is_lh_instruction),
		.is_lw_instruction(is_lw_instruction),
		.is_lbu_instruction(is_lbu_instruction),
		.is_lhu_instruction(is_lhu_instruction),
		.is_sb_instruction(is_sb_instruction),
		.is_sh_instruction(is_sh_instruction),
		.is_sw_instruction(is_sw_instruction),
		.is_p_lb_i_inc_instruction(is_p_lb_i_inc_instruction),
		.is_p_lbu_i_inc_instruction(is_p_lbu_i_inc_instruction),
		.is_p_lh_i_inc_instruction(is_p_lh_i_inc_instruction),
		.is_p_lhu_i_inc_instruction(is_p_lhu_i_inc_instruction),
		.is_p_lw_i_inc_instruction(is_p_lw_i_inc_instruction),
		.is_p_lb_r_inc_instruction(is_p_lb_r_inc_instruction),
		.is_p_lbu_r_inc_instruction(is_p_lbu_r_inc_instruction),
		.is_p_lh_r_inc_instruction(is_p_lh_r_inc_instruction),
		.is_p_lhu_r_inc_instruction(is_p_lhu_r_inc_instruction),
		.is_p_lw_r_inc_instruction(is_p_lw_r_inc_instruction),
		.is_p_lb_r_instruction(is_p_lb_r_instruction),
		.is_p_lbu_r_instruction(is_p_lbu_r_instruction),
		.is_p_lh_r_instruction(is_p_lh_r_instruction),
		.is_p_lhu_r_instruction(is_p_lhu_r_instruction),
		.is_p_lw_r_instruction(is_p_lw_r_instruction),
		.is_p_sb_i_inc_instruction(is_p_sb_i_inc_instruction),
		.is_p_sh_i_inc_instruction(is_p_sh_i_inc_instruction),
		.is_p_sw_i_inc_instruction(is_p_sw_i_inc_instruction),
		.is_p_sb_r_inc_instruction(is_p_sb_r_inc_instruction),
		.is_p_sh_r_inc_instruction(is_p_sh_r_inc_instruction),
		.is_p_sw_r_inc_instruction(is_p_sw_r_inc_instruction),
		.is_p_sb_r_instruction(is_p_sb_r_instruction),
		.is_p_sh_r_instruction(is_p_sh_r_instruction),
		.is_p_sw_r_instruction(is_p_sw_r_instruction),
		.is_add_instruction(is_add_instruction),
		.is_addi_instruction(is_addi_instruction),
		.is_p_clip_instruction(is_p_clip_instruction),
		.is_p_abs_instruction(is_p_abs_instruction),
		.is_p_addn_instruction(is_p_addn_instruction),
		.is_p_addun_instruction(is_p_addun_instruction),
		.is_p_addrn_instruction(is_p_addrn_instruction),
		.is_p_addurn_instruction(is_p_addurn_instruction),
		.is_p_subn_instruction(is_p_subn_instruction),
		.is_p_subun_instruction(is_p_subun_instruction),
		.is_p_subrn_instruction(is_p_subrn_instruction),
		.is_p_suburn_instruction(is_p_suburn_instruction),
		.is_p_addnr_instruction(is_p_addnr_instruction),
		.is_p_addunr_instruction(is_p_addunr_instruction),
		.is_p_addrnr_instruction(is_p_addrnr_instruction),
		.is_p_addurnr_instruction(is_p_addurnr_instruction),
		.is_p_subnr_instruction(is_p_subnr_instruction),
		.is_p_subunr_instruction(is_p_subunr_instruction),
		.is_p_subrnr_instruction(is_p_subrnr_instruction),
		.is_p_suburnr_instruction(is_p_suburnr_instruction),
		.is_pv_add_h_instruction(is_pv_add_h_instruction),
		.is_pv_add_b_instruction(is_pv_add_b_instruction),
		.is_pv_sub_h_instruction(is_pv_sub_h_instruction),
		.is_pv_sub_b_instruction(is_pv_sub_b_instruction),
		.is_jal_instruction(is_jal_instruction),
		.is_jalr_instruction(is_jalr_instruction),
		.is_beq_instruction(is_beq_instruction),
		.is_bne_instruction(is_bne_instruction),
		.is_blt_instruction(is_blt_instruction),
		.is_bge_instruction(is_bge_instruction),
		.is_bltu_instruction(is_bltu_instruction),
		.is_bgeu_instruction(is_bgeu_instruction),
		.has_operand_rs1(has_operand_rs1),
		.has_operand_rs2(has_operand_rs2),
		.has_operand_rd(has_operand_rd),
		.has_operand_csr(has_operand_csr),
		.operand_rs1(operand_rs1),
		.operand_rs2(operand_rs2),
		.operand_rd(operand_rd),
		.operand_csr(operand_csr),
		.operand_imm_p_lb_i_inc(operand_imm_p_lb_i_inc),
		.operand_imm_p_lbu_i_inc(operand_imm_p_lbu_i_inc),
		.operand_imm_p_lh_i_inc(operand_imm_p_lh_i_inc),
		.operand_imm_p_lhu_i_inc(operand_imm_p_lhu_i_inc),
		.operand_imm_p_lw_i_inc(operand_imm_p_lw_i_inc),
		.operand_imm_p_sb_i_inc(operand_imm_p_sb_i_inc),
		.operand_imm_p_sh_i_inc(operand_imm_p_sh_i_inc),
		.operand_imm_p_sw_i_inc(operand_imm_p_sw_i_inc)
	);

	wire mode_adder = (mode_of_operation_param == 2'd 0);
	wire mode_decoder = (mode_of_operation_param == 2'd 1);
	wire mode_lsu = (mode_of_operation_param == 2'd 2);

	wire is_post_increment_load = |{
		is_p_lb_r_inc_instruction,
		is_p_lbu_r_inc_instruction,
		is_p_lh_r_inc_instruction,
		is_p_lhu_r_inc_instruction,
		is_p_lw_r_inc_instruction
	};

	wire is_post_increment_store = |{
		is_p_sb_r_inc_instruction,
		is_p_sh_r_inc_instruction,
		is_p_sw_r_inc_instruction
	};

	wire is_control_flow_instruction = |{
		is_jal_instruction,
		is_jalr_instruction,
		is_beq_instruction,
		is_bne_instruction,
		is_blt_instruction,
		is_bge_instruction,
		is_bltu_instruction,
		is_bgeu_instruction
	};

	assign valid_adder_constraints_c = !mode_adder || &{
		id_instruction_valid,
		opcode_valid,
		|{
			is_add_instruction,
			is_addi_instruction,
			is_sb_instruction,
			is_sh_instruction,
			is_sw_instruction,
			// PULP specific
			is_p_clip_instruction,
			is_p_abs_instruction,
			is_p_addn_instruction,
			is_p_addun_instruction,
			is_p_addrn_instruction,
			is_p_addurn_instruction,
			is_p_subn_instruction,
			is_p_subun_instruction,
			is_p_subrn_instruction,
			is_p_suburn_instruction,
			is_p_addnr_instruction,
			is_p_addunr_instruction,
			is_p_addrnr_instruction,
			is_p_addurnr_instruction,
			is_p_subnr_instruction,
			is_p_subunr_instruction,
			is_p_subrnr_instruction,
			is_p_suburnr_instruction,
			is_pv_add_h_instruction,
			is_pv_add_b_instruction,
			is_pv_sub_h_instruction,
			is_pv_sub_b_instruction,
			// NEW
			is_sub_instruction,
			is_slt_instruction,
			is_sltu_instruction,
			is_p_slet_instruction,
			is_p_sletu_instruction,
			is_p_min_instruction,
			is_p_minu_instruction,
			is_p_max_instruction,
			is_p_maxu_instruction,
			is_p_clipr_instruction,
			is_p_clipur_instruction,
			is_pv_add_sc_h_instruction,
			is_pv_add_sc_b_instruction,
			is_pv_sub_sc_h_instruction,
			is_pv_sub_sc_b_instruction,
			is_pv_avg_h_instruction,
			is_pv_avg_sc_h_instruction,
			is_pv_avg_b_instruction,
			is_pv_avg_sc_b_instruction,
			is_pv_avgu_h_instruction,
			is_pv_avgu_sc_h_instruction,
			is_pv_avgu_b_instruction,
			is_pv_avgu_sc_b_instruction,
			is_pv_min_h_instruction,
			is_pv_min_sc_h_instruction,
			is_pv_min_b_instruction,
			is_pv_min_sc_b_instruction,
			is_pv_minu_h_instruction,
			is_pv_minu_sc_h_instruction,
			is_pv_minu_b_instruction,
			is_pv_minu_sc_b_instruction,
			is_pv_max_h_instruction,
			is_pv_max_sc_h_instruction,
			is_pv_max_b_instruction,
			is_pv_max_sc_b_instruction,
			is_pv_maxu_h_instruction,
			is_pv_maxu_sc_h_instruction,
			is_pv_maxu_b_instruction,
			is_pv_maxu_sc_b_instruction,
			is_pv_dotup_h_instruction,
			is_pv_dotup_sc_h_instruction,
			is_pv_dotup_b_instruction,
			is_pv_dotup_sc_b_instruction,
			is_pv_dotusp_h_instruction,
			is_pv_dotusp_sc_h_instruction,
			is_pv_dotusp_b_instruction,
			is_pv_dotusp_sc_b_instruction,
			is_pv_dotsp_h_instruction,
			is_pv_dotsp_sc_h_instruction,
			is_pv_dotsp_b_instruction,
			is_pv_dotsp_sc_b_instruction,
			is_pv_sdotup_h_instruction,
			is_pv_sdotup_sc_h_instruction,
			is_pv_sdotup_b_instruction,
			is_pv_sdotup_sc_b_instruction,
			is_pv_sdotusp_h_instruction,
			is_pv_sdotusp_sc_h_instruction,
			is_pv_sdotusp_b_instruction,
			is_pv_sdotusp_sc_b_instruction,
			is_pv_sdotsp_h_instruction,
			is_pv_sdotsp_sc_h_instruction,
			is_pv_sdotsp_b_instruction,
			is_pv_sdotsp_sc_b_instruction,
			is_pv_shuffle_h_instruction,
			is_pv_shuffle_b_instruction,
			is_pv_shuffle2_h_instruction,
			is_pv_shuffle2_b_instruction,
			is_pv_cmpeq_h_instruction,
			is_pv_cmpeq_sc_h_instruction,
			is_pv_cmpeq_b_instruction,
			is_pv_cmpeq_sc_b_instruction,
			is_pv_cmpne_h_instruction,
			is_pv_cmpne_sc_h_instruction,
			is_pv_cmpne_b_instruction,
			is_pv_cmpne_sc_b_instruction,
			is_pv_cmpgt_h_instruction,
			is_pv_cmpgt_sc_h_instruction,
			is_pv_cmpgt_b_instruction,
			is_pv_cmpgt_sc_b_instruction,
			is_pv_cmpge_h_instruction,
			is_pv_cmpge_sc_h_instruction,
			is_pv_cmpge_b_instruction,
			is_pv_cmpge_sc_b_instruction,
			is_pv_cmplt_h_instruction,
			is_pv_cmplt_sc_h_instruction,
			is_pv_cmplt_b_instruction,
			is_pv_cmplt_sc_b_instruction,
			is_pv_cmple_h_instruction,
			is_pv_cmple_sc_h_instruction,
			is_pv_cmple_b_instruction,
			is_pv_cmple_sc_b_instruction,
			is_pv_cmpgtu_h_instruction,
			is_pv_cmpgtu_sc_h_instruction,
			is_pv_cmpgtu_b_instruction,
			is_pv_cmpgtu_sc_b_instruction,
			is_pv_cmpgeu_h_instruction,
			is_pv_cmpgeu_sc_h_instruction,
			is_pv_cmpgeu_b_instruction,
			is_pv_cmpgeu_sc_b_instruction,
			is_pv_cmpltu_h_instruction,
			is_pv_cmpltu_sc_h_instruction,
			is_pv_cmpltu_b_instruction,
			is_pv_cmpltu_sc_b_instruction,
			is_pv_cmpleu_h_instruction,
			is_pv_cmpleu_sc_h_instruction,
			is_pv_cmpleu_b_instruction,
			is_pv_cmpleu_sc_b_instruction,
			is_slti_instruction,
			is_sltiu_instruction,
			is_p_machhsn_instruction,
			is_p_macsrn_instruction,
			is_p_machhsrn_instruction,
			is_p_macun_instruction,
			is_p_machhun_instruction,
			is_p_macurn_instruction,
			is_p_machhurn_instruction,
			is_p_macsn_instruction,
			is_p_clipu_instruction,
			is_pv_add_sci_h_instruction,
			is_pv_add_sci_b_instruction,
			is_pv_sub_sci_h_instruction,
			is_pv_sub_sci_b_instruction,
			is_pv_avg_sci_h_instruction,
			is_pv_avg_sci_b_instruction,
			is_pv_min_sci_h_instruction,
			is_pv_min_sci_b_instruction,
			is_pv_max_sci_h_instruction,
			is_pv_max_sci_b_instruction,
			is_pv_dotusp_sci_h_instruction,
			is_pv_dotusp_sci_b_instruction,
			is_pv_dotsp_sci_h_instruction,
			is_pv_dotsp_sci_b_instruction,
			is_pv_sdotusp_sci_h_instruction,
			is_pv_sdotusp_sci_b_instruction,
			is_pv_sdotsp_sci_h_instruction,
			is_pv_sdotsp_sci_b_instruction,
			is_pv_cmple_sci_b_instruction,
			is_pv_cmple_sci_h_instruction,
			is_pv_cmplt_sci_b_instruction,
			is_pv_cmplt_sci_h_instruction,
			is_pv_cmpge_sci_b_instruction,
			is_pv_cmpge_sci_h_instruction,
			is_pv_cmpgt_sci_b_instruction,
			is_pv_cmpgt_sci_h_instruction,
			is_pv_cmpne_sci_b_instruction,
			is_pv_cmpne_sci_h_instruction,
			is_pv_cmpeq_sci_b_instruction,
			is_pv_cmpeq_sci_h_instruction,
			is_pv_shuffle_sci_h_instruction,
			is_pv_shufflei0_sci_b_instruction,
			is_pv_shufflei1_sci_b_instruction,
			is_pv_shufflei2_sci_b_instruction,
			is_pv_shufflei3_sci_b_instruction,
			is_pv_avgu_sci_h_instruction,
			is_pv_avgu_sci_b_instruction,
			is_pv_minu_sci_h_instruction,
			is_pv_minu_sci_b_instruction,
			is_pv_maxu_sci_h_instruction,
			is_pv_maxu_sci_b_instruction,
			is_pv_dotup_sci_h_instruction,
			is_pv_dotup_sci_b_instruction,
			is_pv_sdotup_sci_h_instruction,
			is_pv_sdotup_sci_b_instruction,
			is_pv_cmpleu_sci_b_instruction,
			is_pv_cmpleu_sci_h_instruction,
			is_pv_cmpltu_sci_b_instruction,
			is_pv_cmpltu_sci_h_instruction,
			is_pv_cmpgeu_sci_b_instruction,
			is_pv_cmpgeu_sci_h_instruction,
			is_pv_cmpgtu_sci_b_instruction,
			is_pv_cmpgtu_sci_h_instruction,
			is_pv_abs_h_instruction,
			is_pv_abs_b_instruction
		},
		// PC value shall increase
		vcm_init || (if_pc == (if_pc_old + 32'd 4)),
		// Never miss on fetch stage
		if_miss == 1'd 0,
		// Do not cause any exceptions
		id_exc_cause == 5'd 0,
		// Do not stall
		do_stall_signals == 2'd 0,
		// Do not interrupt
		do_interrupt_signals == 2'd 0
	};

	assign valid_decoder_constraints_c = !mode_decoder || &{
		id_instruction_valid,
		opcode_valid,
		// Prohibit branches & jumps
		is_control_flow_instruction == 1'd 0,
		// PC value shall increase
		vcm_init || (if_pc == (if_pc_old + 32'd 4)),
		// Never miss on fetch stage
		if_miss == 1'd 0,
		// Do not cause any exceptions
		id_exc_cause == 5'd 0,
		// Do not stall
		do_stall_signals == 2'd 0,
		// Do not interrupt
		do_interrupt_signals == 2'd 0,
		// if its a CSR -> appropriate range
		!has_operand_csr || |{
			operand_csr == 12'b 00_11_00_000000, // MSTATUS
			operand_csr == 12'b 00_11_00_000101, // MTVEC
			operand_csr == 12'b 00_11_01_000001, // MEPC
			operand_csr == 12'b 01_11_01_000010, // MCAUSE
			operand_csr == 12'b 01_11_00_0XXXXX, // PCCRs
			operand_csr == 12'b 01_11_10_100000, // PCER
			operand_csr == 12'b 01_11_10_100001, // PCMR
			operand_csr == 12'b 01_11_10_110XXX, // HWLP
			operand_csr == 12'b 11_00_00_010000, // PRIVLV
			operand_csr == 12'b 00_00_00_010100, // UHARTID
			operand_csr == 12'b 11_11_00_010100, // MHARTID
			operand_csr == 12'b 01_11_10_110000, // DCSR
			operand_csr == 12'b 01_11_10_110001, // DPC
			operand_csr == 12'b 01_11_10_110010, // DSCRATCH0
			operand_csr == 12'b 01_11_10_110011  // DSCRATCH1
		}
	};

	assign valid_lsu_constraints_c = !mode_lsu || &{
		id_instruction_valid,
		opcode_valid,
		|{
			is_lb_instruction,
			is_lh_instruction,
			is_lw_instruction,
			is_lbu_instruction,
			is_lhu_instruction,
			is_sb_instruction,
			is_sh_instruction,
			is_sw_instruction,
			is_p_lb_i_inc_instruction,
			is_p_lbu_i_inc_instruction,
			is_p_lh_i_inc_instruction,
			is_p_lhu_i_inc_instruction,
			is_p_lw_i_inc_instruction,
			is_p_lb_r_inc_instruction,
			is_p_lbu_r_inc_instruction,
			is_p_lh_r_inc_instruction,
			is_p_lhu_r_inc_instruction,
			is_p_lw_r_inc_instruction,
			is_p_lb_r_instruction,
			is_p_lbu_r_instruction,
			is_p_lh_r_instruction,
			is_p_lhu_r_instruction,
			is_p_lw_r_instruction,
			is_p_sb_i_inc_instruction,
			is_p_sh_i_inc_instruction,
			is_p_sw_i_inc_instruction,
			is_p_sb_r_inc_instruction,
			is_p_sh_r_inc_instruction,
			is_p_sw_r_inc_instruction,
			is_p_sb_r_instruction,
			is_p_sh_r_instruction,
			is_p_sw_r_instruction,
			is_add_instruction,
			is_addi_instruction,
			is_p_clip_instruction,
			is_p_abs_instruction,
			is_p_addn_instruction,
			is_p_addun_instruction,
			is_p_addrn_instruction,
			is_p_addurn_instruction,
			is_p_subn_instruction,
			is_p_subun_instruction,
			is_p_subrn_instruction,
			is_p_suburn_instruction,
			is_p_addnr_instruction,
			is_p_addunr_instruction,
			is_p_addrnr_instruction,
			is_p_addurnr_instruction,
			is_p_subnr_instruction,
			is_p_subunr_instruction,
			is_p_subrnr_instruction,
			is_p_suburnr_instruction,
			is_pv_add_h_instruction,
			is_pv_add_b_instruction,
			is_pv_sub_h_instruction,
			is_pv_sub_b_instruction
		},
		// PC value shall increase
		vcm_init || (if_pc == (if_pc_old + 32'd 4)),
		// Never miss on fetch stage
		if_miss == 1'd 0,
		// Do not cause any exceptions
		id_exc_cause == 5'd 0,
		// Do not stall
		do_stall_signals == 2'd 0,
		// Do not interrupt
		do_interrupt_signals == 2'd 0,
		// Valid dbus range according to linker script, change in case of different environment
		(data_addr_o >= 32'h 200000) && (data_addr_o <= 32'h 240000),
		// All post increment instructions must have an increment 0 due to repeatability
		!is_post_increment_load || (operand_rs2 == 5'd 0),
		!is_post_increment_store || (operand_rd == 5'd 0),
		// Post-Increment immediates special case
		!is_p_lb_i_inc_instruction || (operand_imm_p_lb_i_inc == 12'd 0),
		!is_p_lbu_i_inc_instruction || (operand_imm_p_lbu_i_inc == 12'd 0),
		!is_p_lh_i_inc_instruction || (operand_imm_p_lh_i_inc == 12'd 0),
		!is_p_lhu_i_inc_instruction || (operand_imm_p_lhu_i_inc == 12'd 0),
		!is_p_lw_i_inc_instruction || (operand_imm_p_lw_i_inc == 12'd 0),
		!is_p_sb_i_inc_instruction || (operand_imm_p_sb_i_inc == 12'd 0),
		!is_p_sh_i_inc_instruction || (operand_imm_p_sh_i_inc == 12'd 0),
		!is_p_sw_i_inc_instruction || (operand_imm_p_sw_i_inc == 12'd 0),
		// Derived from logic-sim
		ex_valid_i == 1'b 1,
		data_misaligned_ex_i == 1'b 0,
		data_err_i == 1'b 0
	};

endmodule

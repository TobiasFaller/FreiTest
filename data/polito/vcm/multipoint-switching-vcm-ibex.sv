`default_nettype none

`include "isa/ibex-instructions.v"

module ibex_vcm(
	input logic clk_i_g ,
	input logic rst_ni_g ,
	input logic test_en_i_g ,
	input logic scan_rst_ni_g ,
	input logic [31:0] hart_id_i_g ,
	input logic [31:0] boot_addr_i_g ,
	input logic [31:0] instr_addr_o_g ,
	input logic instr_gnt_i_g ,
	input logic instr_rvalid_i_g ,
	input logic [31:0] instr_rdata_i_g ,
	input logic [6:0] instr_rdata_intg_i_g ,
	input logic instr_err_i_g ,
	input logic irq_software_i_g ,
	input logic irq_timer_i_g ,
	input logic irq_external_i_g ,
	input logic [14:0] irq_fast_i_g ,
	input logic irq_nm_i_g ,
	input logic scramble_key_valid_i_g ,
	input logic [127:0] scramble_key_i_g ,
	input logic [63:0] scramble_nonce_i_g ,
	input logic [3:0] fetch_enable_i_g ,
	input logic debug_req_i_g ,
	input logic [31:0] \u_ibex_core/if_stage_i/gen_prefetch_buffer_prefetch_buffer_i/rdata_o_g ,
	input logic \u_ibex_core/if_stage_i/compressed_decoder_i/illegal_instr_o_g ,
	input logic \u_ibex_core/if_stage_i/pmp_err_if_i_g ,
	input logic \u_ibex_core/if_stage_i/instr_rvalid_i_g ,
	input logic \u_ibex_core/if_stage_i/instr_bus_err_i_g ,
	input logic \u_ibex_core/if_stage_i/ic_scr_key_valid_i_g ,
	input logic [1:0] \u_ibex_core/if_stage_i/exc_pc_mux_i_g ,
	input logic [31:0] \u_ibex_core/if_stage_i/pc_if_o_g ,
	input logic \u_ibex_core/if_stage_i/compressed_decoder_i/is_compressed_o_g ,
	input logic \u_ibex_core/id_stage_i/lsu_store_err_i_g ,
	input logic \u_ibex_core/id_stage_i/lsu_load_err_i_g ,
	input logic \u_ibex_core/id_stage_i/instr_valid_i_g ,
	input logic \u_ibex_core/id_stage_i/controller_i/illegal_insn_i_g ,
	input logic \u_ibex_core/id_stage_i/controller_i/stall_id_i_g ,
	input logic \u_ibex_core/id_stage_i/controller_i/load_err_i_g ,
	input logic \u_ibex_core/id_stage_i/controller_i/store_err_i_g ,
	input logic [17:0] \u_ibex_core/id_stage_i/controller_i/irqs_i_g ,
	input logic [1:0] \u_ibex_core/id_stage_i/controller_i/priv_mode_i_g , // 2'h 3
	input logic \u_ibex_core/id_stage_i/controller_i/flush_id_o_g ,
	input logic \u_ibex_core/id_stage_i/controller_i/wb_exception_o_g ,
	input logic [5:0] \u_ibex_core/id_stage_i/controller_i/exc_cause_o_g ,
	input logic [1:0] mode_of_operation_param , 	// 0: adder, 1: compressed decoder, 2: decoder, 4: lsu
	input logic vcm_init ,
	output logic valid_adder_constraints_c ,
	output logic valid_compressed_decoder_constraints_c ,
	output logic valid_register_usage_for_compressed_instructions_c ,
	output logic primary_inputs_c ,
	output logic if_c ,
	output logic id_c ,
	output logic valid_pc_increment_c ,
	output logic valid_instruction_memory_region_c ,
	output logic id_controller_c

);

	wire [31:0]  opcode;
	wire         opcode_valid;
	wire         is_lui_instruction;
	wire         is_auipc_instruction;
	wire         is_jal_instruction;
	wire         is_jalr_instruction;
	wire         is_beq_instruction;
	wire         is_bne_instruction;
	wire         is_blt_instruction;
	wire         is_bge_instruction;
	wire         is_bltu_instruction;
	wire         is_bgeu_instruction;
	wire         is_lb_instruction;
	wire         is_lh_instruction;
	wire         is_lw_instruction;
	wire         is_lbu_instruction;
	wire         is_lhu_instruction;
	wire         is_sb_instruction;
	wire         is_sh_instruction;
	wire         is_sw_instruction;
	wire         is_addi_instruction;
	wire         is_slti_instruction;
	wire         is_sltiu_instruction;
	wire         is_xori_instruction;
	wire         is_ori_instruction;
	wire         is_andi_instruction;
	wire         is_slli_instruction;
	wire         is_srli_instruction;
	wire         is_srai_instruction;
	wire         is_add_instruction;
	wire         is_sub_instruction;
	wire         is_sll_instruction;
	wire         is_slt_instruction;
	wire         is_sltu_instruction;
	wire         is_xor_instruction;
	wire         is_srl_instruction;
	wire         is_sra_instruction;
	wire         is_or_instruction;
	wire         is_and_instruction;
	wire         is_fence_instruction;
	wire         is_ecall_instruction;
	wire         is_ebreak_instruction;
	wire         is_mul_instruction;
	wire         is_mulh_instruction;
	wire         is_mulhsu_instruction;
	wire         is_mulhu_instruction;
	wire         is_div_instruction;
	wire         is_divu_instruction;
	wire         is_rem_instruction;
	wire         is_remu_instruction;
	wire         is_caddi4spn_instruction;
	wire         is_clw_instruction;
	wire         is_csw_instruction;
	wire         is_caddi_instruction;
	wire         is_cnop_instruction;
	wire         is_cjal_instruction;
	wire         is_cli_instruction;
	wire         is_clui_instruction;
	wire         is_caddi16sp_instruction;
	wire         is_csrli_instruction;
	wire         is_csrai_instruction;
	wire         is_candi_instruction;
	wire         is_csub_instruction;
	wire         is_cxor_instruction;
	wire         is_cor_instruction;
	wire         is_cand_instruction;
	wire         is_cj_instruction;
	wire         is_cbeqz_instruction;
	wire         is_cbnez_instruction;
	wire         is_cslli_instruction;
	wire         is_clwsp_instruction;
	wire         is_cmv_instruction;
	wire         is_cjr_instruction;
	wire         is_cadd_instruction;
	wire         is_cjalr_instruction;
	wire         is_cebreak_instruction;
	wire         is_cswsp_instruction;
	wire         is_csrrw_instruction;
	wire         is_csrrs_instruction;
	wire         is_csrrc_instruction;
	wire         is_csrrwi_instruction;
	wire         is_csrrsi_instruction;
	wire         is_csrrci_instruction;
	wire         has_operand_csr;
	wire         has_operand_fm;
	wire         has_operand_imm;
	wire         has_operand_nzimm;
	wire         has_operand_nzuimm;
	wire         has_operand_pred;
	wire         has_operand_rd;
	wire         has_operand_rs1;
	wire         has_operand_rs2;
	wire         has_operand_shamt;
	wire         has_operand_succ;
	wire         has_operand_uimm;
	wire         has_operand_zimm;
	wire [4:0]   operand_rd;
	wire [4:0]   operand_rs1;
	wire [4:0]   operand_rs2;
	wire [11:0]  operand_csr;
	wire [3:0]   operand_fm_fence;
	wire [31:12] operand_imm_lui;
	wire [31:12] operand_imm_auipc;
	wire [20:1]  operand_imm_jal;
	wire [11:0]  operand_imm_jalr;
	wire [12:1]  operand_imm_beq;
	wire [12:1]  operand_imm_bne;
	wire [12:1]  operand_imm_blt;
	wire [12:1]  operand_imm_bge;
	wire [12:1]  operand_imm_bltu;
	wire [12:1]  operand_imm_bgeu;
	wire [11:0]  operand_imm_lb;
	wire [11:0]  operand_imm_lh;
	wire [11:0]  operand_imm_lw;
	wire [11:0]  operand_imm_lbu;
	wire [11:0]  operand_imm_lhu;
	wire [11:0]  operand_imm_sb;
	wire [11:0]  operand_imm_sh;
	wire [11:0]  operand_imm_sw;
	wire [11:0]  operand_imm_addi;
	wire [11:0]  operand_imm_slti;
	wire [11:0]  operand_imm_sltiu;
	wire [11:0]  operand_imm_xori;
	wire [11:0]  operand_imm_ori;
	wire [11:0]  operand_imm_andi;
	wire [9:2]   operand_imm_caddi4spn;
	wire [5:0]   operand_imm_caddi;
	wire [11:1]  operand_imm_cjal;
	wire [5:0]   operand_imm_cli;
	wire [17:12] operand_imm_clui;
	wire [5:0]   operand_imm_candi;
	wire [11:1]  operand_imm_cj;
	wire [8:1]   operand_imm_cbeqz;
	wire [8:1]   operand_imm_cbnez;
	wire [5:0]   operand_nzimm_cnop;
	wire [9:4]   operand_nzimm_caddi16sp;
	wire [4:0]   operand_nzuimm_cslli;
	wire [3:0]   operand_pred_fence;
	wire [4:0]   operand_shamt_slli;
	wire [4:0]   operand_shamt_srli;
	wire [4:0]   operand_shamt_srai;
	wire [4:0]   operand_shamt_csrli;
	wire [4:0]   operand_shamt_csrai;
	wire [3:0]   operand_succ_fence;
	wire [6:2]   operand_uimm_clw;
	wire [6:2]   operand_uimm_csw;
	wire [7:2]   operand_uimm_clwsp;
	wire [7:2]   operand_uimm_cswsp;
	wire [4:0]   operand_zimm_csrrwi;
	wire [4:0]   operand_zimm_csrrsi;
	wire [4:0]   operand_zimm_csrrci;

	assign opcode = \u_ibex_core/if_stage_i/gen_prefetch_buffer_prefetch_buffer_i/rdata_o_g ;
	rv32imc_zicsr rv_inst(
		.opcode                   ( opcode                   ),
		.opcode_valid             ( opcode_valid             ),
		.is_lui_instruction       ( is_lui_instruction       ),
		.is_auipc_instruction     ( is_auipc_instruction     ),
		.is_jal_instruction       ( is_jal_instruction       ),
		.is_jalr_instruction      ( is_jalr_instruction      ),
		.is_beq_instruction       ( is_beq_instruction       ),
		.is_bne_instruction       ( is_bne_instruction       ),
		.is_blt_instruction       ( is_blt_instruction       ),
		.is_bge_instruction       ( is_bge_instruction       ),
		.is_bltu_instruction      ( is_bltu_instruction      ),
		.is_bgeu_instruction      ( is_bgeu_instruction      ),
		.is_lb_instruction        ( is_lb_instruction        ),
		.is_lh_instruction        ( is_lh_instruction        ),
		.is_lw_instruction        ( is_lw_instruction        ),
		.is_lbu_instruction       ( is_lbu_instruction       ),
		.is_lhu_instruction       ( is_lhu_instruction       ),
		.is_sb_instruction        ( is_sb_instruction        ),
		.is_sh_instruction        ( is_sh_instruction        ),
		.is_sw_instruction        ( is_sw_instruction        ),
		.is_addi_instruction      ( is_addi_instruction      ),
		.is_slti_instruction      ( is_slti_instruction      ),
		.is_sltiu_instruction     ( is_sltiu_instruction     ),
		.is_xori_instruction      ( is_xori_instruction      ),
		.is_ori_instruction       ( is_ori_instruction       ),
		.is_andi_instruction      ( is_andi_instruction      ),
		.is_slli_instruction      ( is_slli_instruction      ),
		.is_srli_instruction      ( is_srli_instruction      ),
		.is_srai_instruction      ( is_srai_instruction      ),
		.is_add_instruction       ( is_add_instruction       ),
		.is_sub_instruction       ( is_sub_instruction       ),
		.is_sll_instruction       ( is_sll_instruction       ),
		.is_slt_instruction       ( is_slt_instruction       ),
		.is_sltu_instruction      ( is_sltu_instruction      ),
		.is_xor_instruction       ( is_xor_instruction       ),
		.is_srl_instruction       ( is_srl_instruction       ),
		.is_sra_instruction       ( is_sra_instruction       ),
		.is_or_instruction        ( is_or_instruction        ),
		.is_and_instruction       ( is_and_instruction       ),
		.is_fence_instruction     ( is_fence_instruction     ),
		.is_ecall_instruction     ( is_ecall_instruction     ),
		.is_ebreak_instruction    ( is_ebreak_instruction    ),
		.is_mul_instruction       ( is_mul_instruction       ),
		.is_mulh_instruction      ( is_mulh_instruction      ),
		.is_mulhsu_instruction    ( is_mulhsu_instruction    ),
		.is_mulhu_instruction     ( is_mulhu_instruction     ),
		.is_div_instruction       ( is_div_instruction       ),
		.is_divu_instruction      ( is_divu_instruction      ),
		.is_rem_instruction       ( is_rem_instruction       ),
		.is_remu_instruction      ( is_remu_instruction      ),
		.is_caddi4spn_instruction ( is_caddi4spn_instruction ),
		.is_clw_instruction       ( is_clw_instruction       ),
		.is_csw_instruction       ( is_csw_instruction       ),
		.is_caddi_instruction     ( is_caddi_instruction     ),
		.is_cnop_instruction      ( is_cnop_instruction      ),
		.is_cjal_instruction      ( is_cjal_instruction      ),
		.is_cli_instruction       ( is_cli_instruction       ),
		.is_clui_instruction      ( is_clui_instruction      ),
		.is_caddi16sp_instruction ( is_caddi16sp_instruction ),
		.is_csrli_instruction     ( is_csrli_instruction     ),
		.is_csrai_instruction     ( is_csrai_instruction     ),
		.is_candi_instruction     ( is_candi_instruction     ),
		.is_csub_instruction      ( is_csub_instruction      ),
		.is_cxor_instruction      ( is_cxor_instruction      ),
		.is_cor_instruction       ( is_cor_instruction       ),
		.is_cand_instruction      ( is_cand_instruction      ),
		.is_cj_instruction        ( is_cj_instruction        ),
		.is_cbeqz_instruction     ( is_cbeqz_instruction     ),
		.is_cbnez_instruction     ( is_cbnez_instruction     ),
		.is_cslli_instruction     ( is_cslli_instruction     ),
		.is_clwsp_instruction     ( is_clwsp_instruction     ),
		.is_cmv_instruction       ( is_cmv_instruction       ),
		.is_cjr_instruction       ( is_cjr_instruction       ),
		.is_cadd_instruction      ( is_cadd_instruction      ),
		.is_cjalr_instruction     ( is_cjalr_instruction     ),
		.is_cebreak_instruction   ( is_cebreak_instruction   ),
		.is_cswsp_instruction     ( is_cswsp_instruction     ),
		.is_csrrw_instruction     ( is_csrrw_instruction     ),
		.is_csrrs_instruction     ( is_csrrs_instruction     ),
		.is_csrrc_instruction     ( is_csrrc_instruction     ),
		.is_csrrwi_instruction    ( is_csrrwi_instruction    ),
		.is_csrrsi_instruction    ( is_csrrsi_instruction    ),
		.is_csrrci_instruction    ( is_csrrci_instruction    ),
		.has_operand_csr          ( has_operand_csr          ),
		.has_operand_fm           ( has_operand_fm           ),
		.has_operand_imm          ( has_operand_imm          ),
		.has_operand_nzimm        ( has_operand_nzimm        ),
		.has_operand_nzuimm       ( has_operand_nzuimm       ),
		.has_operand_pred         ( has_operand_pred         ),
		.has_operand_rd           ( has_operand_rd           ),
		.has_operand_rs1          ( has_operand_rs1          ),
		.has_operand_rs2          ( has_operand_rs2          ),
		.has_operand_shamt        ( has_operand_shamt        ),
		.has_operand_succ         ( has_operand_succ         ),
		.has_operand_uimm         ( has_operand_uimm         ),
		.has_operand_zimm         ( has_operand_zimm         ),
		.operand_rd               ( operand_rd               ),
		.operand_rs1              ( operand_rs1              ),
		.operand_rs2              ( operand_rs2              ),
		.operand_csr              ( operand_csr              ),
		.operand_fm_fence         ( operand_fm_fence         ),
		.operand_imm_lui          ( operand_imm_lui          ),
		.operand_imm_auipc        ( operand_imm_auipc        ),
		.operand_imm_jal          ( operand_imm_jal          ),
		.operand_imm_jalr         ( operand_imm_jalr         ),
		.operand_imm_beq          ( operand_imm_beq          ),
		.operand_imm_bne          ( operand_imm_bne          ),
		.operand_imm_blt          ( operand_imm_blt          ),
		.operand_imm_bge          ( operand_imm_bge          ),
		.operand_imm_bltu         ( operand_imm_bltu         ),
		.operand_imm_bgeu         ( operand_imm_bgeu         ),
		.operand_imm_lb           ( operand_imm_lb           ),
		.operand_imm_lh           ( operand_imm_lh           ),
		.operand_imm_lw           ( operand_imm_lw           ),
		.operand_imm_lbu          ( operand_imm_lbu          ),
		.operand_imm_lhu          ( operand_imm_lhu          ),
		.operand_imm_sb           ( operand_imm_sb           ),
		.operand_imm_sh           ( operand_imm_sh           ),
		.operand_imm_sw           ( operand_imm_sw           ),
		.operand_imm_addi         ( operand_imm_addi         ),
		.operand_imm_slti         ( operand_imm_slti         ),
		.operand_imm_sltiu        ( operand_imm_sltiu        ),
		.operand_imm_xori         ( operand_imm_xori         ),
		.operand_imm_ori          ( operand_imm_ori          ),
		.operand_imm_andi         ( operand_imm_andi         ),
		.operand_imm_caddi4spn    ( operand_imm_caddi4spn    ),
		.operand_imm_caddi        ( operand_imm_caddi        ),
		.operand_imm_cjal         ( operand_imm_cjal         ),
		.operand_imm_cli          ( operand_imm_cli          ),
		.operand_imm_clui         ( operand_imm_clui         ),
		.operand_imm_candi        ( operand_imm_candi        ),
		.operand_imm_cj           ( operand_imm_cj           ),
		.operand_imm_cbeqz        ( operand_imm_cbeqz        ),
		.operand_imm_cbnez        ( operand_imm_cbnez        ),
		.operand_nzimm_cnop       ( operand_nzimm_cnop       ),
		.operand_nzimm_caddi16sp  ( operand_nzimm_caddi16sp  ),
		.operand_nzuimm_cslli     ( operand_nzuimm_cslli     ),
		.operand_pred_fence       ( operand_pred_fence       ),
		.operand_shamt_slli       ( operand_shamt_slli       ),
		.operand_shamt_srli       ( operand_shamt_srli       ),
		.operand_shamt_srai       ( operand_shamt_srai       ),
		.operand_shamt_csrli      ( operand_shamt_csrli      ),
		.operand_shamt_csrai      ( operand_shamt_csrai      ),
		.operand_succ_fence       ( operand_succ_fence       ),
		.operand_uimm_clw         ( operand_uimm_clw         ),
		.operand_uimm_csw         ( operand_uimm_csw         ),
		.operand_uimm_clwsp       ( operand_uimm_clwsp       ),
		.operand_uimm_cswsp       ( operand_uimm_cswsp       ),
		.operand_zimm_csrrwi      ( operand_zimm_csrrwi      ),
		.operand_zimm_csrrsi      ( operand_zimm_csrrsi      ),
		.operand_zimm_csrrci      ( operand_zimm_csrrci      )
	);


	////////////////////////////////////////
	// PRIMARY INPUTS                     //
	// taken directly from the testbench  //
	////////////////////////////////////////
	assign primary_inputs_c = &{
		rst_ni_g               == 1'b1,
		scan_rst_ni_g          == 1'b1,
		test_en_i_g            == 1'b0,
		hart_id_i_g            == 32'h0,
		boot_addr_i_g          == 32'h0,
		irq_software_i_g       == 1'b0,
		irq_timer_i_g          == 1'b0,
		irq_external_i_g       == 1'b0,
		irq_fast_i_g           == 15'b0,
		irq_nm_i_g             == 1'b0,
		scramble_key_valid_i_g == 1'b0,
		scramble_key_i_g       == 128'b0,
		scramble_nonce_i_g     == 64'b0,
		debug_req_i_g          == 1'b0,
		fetch_enable_i_g       == 4'b0101 // refer to rtl/ibex_pkg.sv (bottom of the file) for further information.
	};

	assign valid_instruction_memory_region_c = ( instr_addr_o_g < 32'h000CD07C );

	////////////////////////////////////////
	// INSTRUCTION FETCH                  //
	////////////////////////////////////////
	assign if_c = &{
		\u_ibex_core/if_stage_i/instr_rvalid_i_g                       == 1'b1,
		\u_ibex_core/if_stage_i/compressed_decoder_i/illegal_instr_o_g == 1'b0,
		\u_ibex_core/if_stage_i/exc_pc_mux_i_g                         == 2'b01,
		\u_ibex_core/if_stage_i/pmp_err_if_i_g                         == 1'b0,
		\u_ibex_core/if_stage_i/instr_bus_err_i_g                      == 1'b0,
		\u_ibex_core/if_stage_i/ic_scr_key_valid_i_g                   == 1'b1
	};

	////////////////////////////////////////
    // INSTRUCTION DECODE                 //
	////////////////////////////////////////
	assign id_c = &{
		\u_ibex_core/id_stage_i/lsu_store_err_i_g == 1'b0,
		\u_ibex_core/id_stage_i/lsu_load_err_i_g  == 1'b0,
		\u_ibex_core/id_stage_i/instr_valid_i_g   == 1'b1
	};

	////////////////////////////////////////
	// CONTROLLER 						  //
	////////////////////////////////////////
	assign id_controller_c = &{
		\u_ibex_core/id_stage_i/controller_i/illegal_insn_i_g == 1'b0,
		\u_ibex_core/id_stage_i/controller_i/store_err_i_g    == 1'b0,
		\u_ibex_core/id_stage_i/controller_i/load_err_i_g     == 1'b0,
		\u_ibex_core/id_stage_i/controller_i/irqs_i_g         == 18'b0,
		\u_ibex_core/id_stage_i/controller_i/priv_mode_i_g    == 2'b11,
		\u_ibex_core/id_stage_i/controller_i/wb_exception_o_g == 1'b0,
		\u_ibex_core/id_stage_i/controller_i/exc_cause_o_g    == 5'b0
	} ;

	////////////////////////////////////////
	// PROGRAM COUNTER                    //
	// If previous instruction was 'C'    //
	// then CURRENT_PC = OLD_PC + 2       //
	// else CURRENT_PC = OLD_PC + 4       //
	////////////////////////////////////////
	wire clock = clk_i_g ;
	wire [31:0] pc = \u_ibex_core/if_stage_i/pc_if_o_g ;
	wire is_compressed = \u_ibex_core/if_stage_i/compressed_decoder_i/is_compressed_o_g ;

	reg was_compressed;
	reg [31:0] previous_pc;
	always_ff @(posedge clock) begin
		was_compressed <= is_compressed ;
		previous_pc <= pc ;
	end

	assign valid_pc_increment_c = vcm_init || &{
		( !was_compressed || (pc == previous_pc + 32'd 2)),
		(  was_compressed || (pc == previous_pc + 32'd 4))
	};

	////////////////////////////////////////
	// ADDER SPECIFIC                     //
	////////////////////////////////////////
	wire target_adder = (mode_of_operation_param == 2'd0);
	wire is_desired_adder_instruction = |{
		is_add_instruction,
		is_sub_instruction,
		is_addi_instruction,
		is_slti_instruction,
		is_sltiu_instruction,
		is_cnop_instruction,
		// Compressed //
		is_cli_instruction,
		is_cadd_instruction,
		is_csub_instruction,
		is_caddi_instruction,
		is_caddi4spn_instruction,
		is_caddi16sp_instruction,
		is_cnop_instruction,
		is_cmv_instruction
	};

	assign valid_adder_constraints_c = !(vcm_init && target_adder) || is_desired_adder_instruction;

	////////////////////////////////////////
	// COMPRESSED DECODER SPECIFIC        //
	////////////////////////////////////////
	wire target_compressed_decoder = (mode_of_operation_param == 2'd1);
	wire is_desired_compressed_decoder_instruction = |{
		is_caddi4spn_instruction,
		is_clw_instruction,
		is_csw_instruction,
		is_caddi_instruction,
		is_cnop_instruction,
		is_cjal_instruction,
		is_cli_instruction,
		is_clui_instruction,
		is_caddi16sp_instruction,
		is_csrli_instruction,
		is_csrai_instruction,
		is_candi_instruction,
		is_csub_instruction,
		is_cxor_instruction,
		is_cor_instruction,
		is_cand_instruction,
		is_cj_instruction,
		is_cbeqz_instruction,
		is_cbnez_instruction,
		is_cslli_instruction,
		is_clwsp_instruction,
		is_cmv_instruction,
		//is_cjr_instruction,
		//is_cjalr_instruction,
		is_cadd_instruction
	};

	wire compressed_instructions_using_only_s0_to_a5 = |{
		//CL instructions rs1', rd'
		is_clw_instruction,
		//CS instructions rs1', rs2'
		is_csw_instruction,
		//CB instructions: rs1'
		is_cbeqz_instruction,
		is_cbnez_instruction,
		//CIW instructions: rd'
		is_caddi4spn_instruction,
		//CB instructions: rd' rs1'
		is_csrli_instruction,
		is_csrai_instruction,
		is_candi_instruction,
		//CA instructions: rs1', rs2', rd'
		is_cand_instruction,
		is_cor_instruction,
		is_cxor_instruction,
		is_csub_instruction
	};

	assign valid_register_usage_for_compressed_instructions_c = !(is_compressed && compressed_instructions_using_only_s0_to_a5) || |{
		(!has_operand_rd  || (operand_rd  >= 3'b 000 && operand_rd  <= 3'b 111)),
		(!has_operand_rs1 || (operand_rs1 >= 3'b 000 && operand_rs1 <= 3'b 111)),
		(!has_operand_rs2 || (operand_rs2 >= 3'b 000 && operand_rs1 <= 3'b 111))
	};

	assign valid_compressed_decoder_constraints_c = !(vcm_init && target_compressed_decoder) || &{
		is_desired_compressed_decoder_instruction,
		// For the branches, abstract from the T/NT outcome and make the target address the very next one.
		// REMARK: The offset is sign-extended and added to the pc to form the branch target address.
		(!is_cbnez_instruction || ({operand_imm_cbnez, 1'b0} == 9'h 02)),
		(!is_cbeqz_instruction || ({operand_imm_cbeqz, 1'b0} == 9'h 02)),
		// For the jumps, simply make the target address the very next one
		(!is_cj_instruction   || ({operand_imm_cj, 1'b0} == 12'h 02)),
		(!is_cjal_instruction || ({operand_imm_cjal, 1'b0} == 12'h 02)),
		1'b1
	};

	// TODO:                              //
	////////////////////////////////////////
	// DECODER SPECIFIC                   //
	////////////////////////////////////////
	wire target_decoder = (mode_of_operation_param == 2'd2);

	// TODO:                              //
	////////////////////////////////////////
	// LOAD STORE UNIT SPECIFIC           //
	////////////////////////////////////////
	wire target_lsu = (mode_of_operation_param == 2'd3);


endmodule

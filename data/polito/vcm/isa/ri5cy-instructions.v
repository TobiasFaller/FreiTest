
module rv32im_zicsr_xpulp2_ri5cy_lui (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [31:12] operand_imm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[6:0] == 7'b 0110111);

	// Assign the decoded operands
	assign operand_imm[31:12] = opcode[31:12];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_auipc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [31:12] operand_imm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[6:0] == 7'b 0010111);

	// Assign the decoded operands
	assign operand_imm[31:12] = opcode[31:12];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_jal (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [20:1] operand_imm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[6:0] == 7'b 1101111);

	// Assign the decoded operands
	assign operand_imm[20:20] = opcode[31:31];
	assign operand_imm[10:1] = opcode[30:21];
	assign operand_imm[11:11] = opcode[20:20];
	assign operand_imm[19:12] = opcode[19:12];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_jalr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1100111);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_beq (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_bne (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_blt (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_bge (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_bltu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_bgeu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm[12:12] = opcode[31:31];
	assign operand_imm[10:5] = opcode[30:25];
	assign operand_imm[4:1] = opcode[11:8];
	assign operand_imm[11:11] = opcode[7:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lb (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lh (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lw (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lbu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lhu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sb (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sh (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sw (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_addi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_slti (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sltiu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_xori (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_ori (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_andi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_slli (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_shamt,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_shamt[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_srli (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_shamt,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_shamt[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_srai (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_shamt,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0010011);

	// Assign the decoded operands
	assign operand_shamt[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_add (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sub (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sll (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_slt (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sltu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_xor (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_srl (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_sra (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_or (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_and (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_fence (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [3:0] operand_fm,
	output wire [3:0] operand_pred,
	output wire [3:0] operand_succ,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0001111);

	// Assign the decoded operands
	assign operand_fm[3:0] = opcode[31:28];
	assign operand_pred[3:0] = opcode[27:24];
	assign operand_succ[3:0] = opcode[23:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_ecall (
	input wire [31:0] opcode,
	output wire opcode_valid,
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000000000000)
		&& (opcode[19:15] == 5'b 00000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[11:7] == 5'b 00000)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands

endmodule

module rv32im_zicsr_xpulp2_ri5cy_ebreak (
	input wire [31:0] opcode,
	output wire opcode_valid,
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000000000001)
		&& (opcode[19:15] == 5'b 00000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[11:7] == 5'b 00000)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands

endmodule

module rv32im_zicsr_xpulp2_ri5cy_mul (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_mulh (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_mulhsu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_mulhu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_div (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_divu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_rem (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_remu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000001)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrw (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrs (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrwi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_zimm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_zimm[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrsi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_zimm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_zimm[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_csrrci (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_csr,
	output wire [4:0] operand_zimm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1110011);

	// Assign the decoded operands
	assign operand_csr[11:0] = opcode[31:20];
	assign operand_zimm[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extract (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_ls3,
	output wire [4:0] operand_ls2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_ls3[4:0] = opcode[29:25];
	assign operand_ls2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extractu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_ls3,
	output wire [4:0] operand_ls2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_ls3[4:0] = opcode[29:25];
	assign operand_ls2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_insert (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_ls3,
	output wire [4:0] operand_ls2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_ls3[4:0] = opcode[29:25];
	assign operand_ls2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_bclr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_ls3,
	output wire [4:0] operand_ls2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_ls3[4:0] = opcode[29:25];
	assign operand_ls2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_bset (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_ls3,
	output wire [4:0] operand_ls2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_ls3[4:0] = opcode[29:25];
	assign operand_ls2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extractr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extractur (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_insertr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_bclrr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_bsetr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_ror (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_ff1 (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_fl1 (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_clb (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_cnt (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_abs (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000001000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_slet (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sletu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_min (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_minu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_max (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_maxu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_exths (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_exthz (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extbs (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_extbz (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000100000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_clip (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_clipu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_clipr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_clipur (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 0110011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addrn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addurn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subrn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_suburn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 01)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addunr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addrnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_addurnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subunr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_subrnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_suburnr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_starti (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_uimmL,
	output wire [0:0] operand_L
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[19:15] == 5'b 00000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_uimmL[11:0] = opcode[31:20];
	assign operand_L[0:0] = opcode[7:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_endi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_uimmL,
	output wire [0:0] operand_L
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[19:15] == 5'b 00000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_uimmL[11:0] = opcode[31:20];
	assign operand_L[0:0] = opcode[7:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_counti (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_uimmL,
	output wire [0:0] operand_L
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[19:15] == 5'b 00000)
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_uimmL[11:0] = opcode[31:20];
	assign operand_L[0:0] = opcode[7:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_count (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [0:0] operand_L
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 000000000000)
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_L[0:0] = opcode[7:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_setup (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_uimmL,
	output wire [4:0] operand_rs1,
	output wire [0:0] operand_L
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_uimmL[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_L[0:0] = opcode[7:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_lp_setupi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_uimmL,
	output wire [4:0] operand_uimmS
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[11:8] == 4'b 0000)
		&& (opcode[7:7] == 1'b 0)
		&& (opcode[6:0] == 7'b 1111011);

	// Assign the decoded operands
	assign operand_uimmL[11:0] = opcode[31:20];
	assign operand_uimmS[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_beqimm (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm1,
	output wire [4:0] operand_imm2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm1[12:12] = opcode[31:31];
	assign operand_imm1[10:5] = opcode[30:25];
	assign operand_imm1[4:1] = opcode[11:8];
	assign operand_imm1[11:11] = opcode[7:7];
	assign operand_imm2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_bneimm (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [12:1] operand_imm1,
	output wire [4:0] operand_imm2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 011)
		&& (opcode[6:0] == 7'b 1100011);

	// Assign the decoded operands
	assign operand_imm1[12:12] = opcode[31:31];
	assign operand_imm1[10:5] = opcode[30:25];
	assign operand_imm1[4:1] = opcode[11:8];
	assign operand_imm1[11:11] = opcode[7:7];
	assign operand_imm2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lb_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lbu_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lh_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lhu_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lw_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_imm[11:0] = opcode[31:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lb_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lbu_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lh_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lhu_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lw_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0001011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lb_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lbu_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lh_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lhu_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_lw_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0000011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulhhu (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulhhun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 01)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulurn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_mulhhurn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 01)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_macsn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_machhsn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_macsrn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 10)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_machhsrn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 11)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_macun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_machhun (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 01)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_macurn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 00)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_machhurn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_uimm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:30] == 2'b 01)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1011011);

	// Assign the decoded operands
	assign operand_uimm[4:0] = opcode[29:25];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sb_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sh_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sw_i_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:0] operand_imm,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[14:12] == 3'b 010)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_imm[11:5] = opcode[31:25];
	assign operand_imm[4:0] = opcode[11:7];
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sb_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sh_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sw_r_inc (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0101011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sb_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sh_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_p_sw_r (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 0100011);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000001)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000001)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000110)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000110)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000011)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000110)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000110)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000011)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000101)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000101)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001110)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001110)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000111)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001110)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001110)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000111)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010010)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001001)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001001)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010110)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010110)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001011)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010110)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010110)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001011)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011010)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001101)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001101)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011110)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011110)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001111)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011110)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011110)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001111)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100010)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100010)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010001)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100010)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100010)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010001)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100110)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100110)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010011)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100110)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100110)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010011)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:26] == 1'b 0)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_add_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00000)
		&& (opcode[26:26] == 1'b 0)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00001)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00001)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00001)
		&& (opcode[26:26] == 1'b 0)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:27] == 5'b 00001)
		&& (opcode[26:25] == 2'b 00)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0000100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sub_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000100)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avg_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0001100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 000110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_min_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0010100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_minu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001100)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_max_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0011100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 001110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_srl_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0100100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sra_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010100)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sll_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0101100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_or_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 010110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_xor_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 0110100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_and_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_abs_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 011100000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_abs_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:20] == 12'b 011100000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_extract_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_extract_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 011110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_extractu_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100100)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_extractu_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_insert_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101100)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_insert_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1000100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1001100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1001100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1001100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1001100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 100110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010000)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010000)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101010)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1010100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1011100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sc_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1011100)
		&& (opcode[14:12] == 3'b 100)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101110)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1011100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sc_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1011100)
		&& (opcode[14:12] == 3'b 101)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 101110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_sci_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 110000)
		&& (opcode[14:12] == 3'b 110)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shufflei0_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 110000)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shufflei1_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 111010)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shufflei2_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 111100)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shufflei3_sci_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:26] == 6'b 111110)
		&& (opcode[14:12] == 3'b 111)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_imm[5:0] = opcode[25:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shuffle2_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100100)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_shuffle2_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1100100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_pack_h (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1101000)
		&& (opcode[14:12] == 3'b 000)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_packhi_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1101100)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule

module rv32im_zicsr_xpulp2_ri5cy_pv_packlo_b (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs2,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:25] == 7'b 1110000)
		&& (opcode[14:12] == 3'b 001)
		&& (opcode[6:0] == 7'b 1010111);

	// Assign the decoded operands
	assign operand_rs2[4:0] = opcode[24:20];
	assign operand_rs1[4:0] = opcode[19:15];
	assign operand_rd[4:0] = opcode[11:7];

endmodule


module rv32im_zicsr_xpulp2_ri5cy (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire is_lui_instruction,
	output wire is_auipc_instruction,
	output wire is_jal_instruction,
	output wire is_jalr_instruction,
	output wire is_beq_instruction,
	output wire is_bne_instruction,
	output wire is_blt_instruction,
	output wire is_bge_instruction,
	output wire is_bltu_instruction,
	output wire is_bgeu_instruction,
	output wire is_lb_instruction,
	output wire is_lh_instruction,
	output wire is_lw_instruction,
	output wire is_lbu_instruction,
	output wire is_lhu_instruction,
	output wire is_sb_instruction,
	output wire is_sh_instruction,
	output wire is_sw_instruction,
	output wire is_addi_instruction,
	output wire is_slti_instruction,
	output wire is_sltiu_instruction,
	output wire is_xori_instruction,
	output wire is_ori_instruction,
	output wire is_andi_instruction,
	output wire is_slli_instruction,
	output wire is_srli_instruction,
	output wire is_srai_instruction,
	output wire is_add_instruction,
	output wire is_sub_instruction,
	output wire is_sll_instruction,
	output wire is_slt_instruction,
	output wire is_sltu_instruction,
	output wire is_xor_instruction,
	output wire is_srl_instruction,
	output wire is_sra_instruction,
	output wire is_or_instruction,
	output wire is_and_instruction,
	output wire is_fence_instruction,
	output wire is_ecall_instruction,
	output wire is_ebreak_instruction,
	output wire is_mul_instruction,
	output wire is_mulh_instruction,
	output wire is_mulhsu_instruction,
	output wire is_mulhu_instruction,
	output wire is_div_instruction,
	output wire is_divu_instruction,
	output wire is_rem_instruction,
	output wire is_remu_instruction,
	output wire is_csrrw_instruction,
	output wire is_csrrs_instruction,
	output wire is_csrrc_instruction,
	output wire is_csrrwi_instruction,
	output wire is_csrrsi_instruction,
	output wire is_csrrci_instruction,
	output wire is_p_extract_instruction,
	output wire is_p_extractu_instruction,
	output wire is_p_insert_instruction,
	output wire is_p_bclr_instruction,
	output wire is_p_bset_instruction,
	output wire is_p_extractr_instruction,
	output wire is_p_extractur_instruction,
	output wire is_p_insertr_instruction,
	output wire is_p_bclrr_instruction,
	output wire is_p_bsetr_instruction,
	output wire is_p_ror_instruction,
	output wire is_p_ff1_instruction,
	output wire is_p_fl1_instruction,
	output wire is_p_clb_instruction,
	output wire is_p_cnt_instruction,
	output wire is_p_abs_instruction,
	output wire is_p_slet_instruction,
	output wire is_p_sletu_instruction,
	output wire is_p_min_instruction,
	output wire is_p_minu_instruction,
	output wire is_p_max_instruction,
	output wire is_p_maxu_instruction,
	output wire is_p_exths_instruction,
	output wire is_p_exthz_instruction,
	output wire is_p_extbs_instruction,
	output wire is_p_extbz_instruction,
	output wire is_p_clip_instruction,
	output wire is_p_clipu_instruction,
	output wire is_p_clipr_instruction,
	output wire is_p_clipur_instruction,
	output wire is_p_addn_instruction,
	output wire is_p_addun_instruction,
	output wire is_p_addrn_instruction,
	output wire is_p_addurn_instruction,
	output wire is_p_subn_instruction,
	output wire is_p_subun_instruction,
	output wire is_p_subrn_instruction,
	output wire is_p_suburn_instruction,
	output wire is_p_addnr_instruction,
	output wire is_p_addunr_instruction,
	output wire is_p_addrnr_instruction,
	output wire is_p_addurnr_instruction,
	output wire is_p_subnr_instruction,
	output wire is_p_subunr_instruction,
	output wire is_p_subrnr_instruction,
	output wire is_p_suburnr_instruction,
	output wire is_lp_starti_instruction,
	output wire is_lp_endi_instruction,
	output wire is_lp_counti_instruction,
	output wire is_lp_count_instruction,
	output wire is_lp_setup_instruction,
	output wire is_lp_setupi_instruction,
	output wire is_p_beqimm_instruction,
	output wire is_p_bneimm_instruction,
	output wire is_p_lb_i_inc_instruction,
	output wire is_p_lbu_i_inc_instruction,
	output wire is_p_lh_i_inc_instruction,
	output wire is_p_lhu_i_inc_instruction,
	output wire is_p_lw_i_inc_instruction,
	output wire is_p_lb_r_inc_instruction,
	output wire is_p_lbu_r_inc_instruction,
	output wire is_p_lh_r_inc_instruction,
	output wire is_p_lhu_r_inc_instruction,
	output wire is_p_lw_r_inc_instruction,
	output wire is_p_lb_r_instruction,
	output wire is_p_lbu_r_instruction,
	output wire is_p_lh_r_instruction,
	output wire is_p_lhu_r_instruction,
	output wire is_p_lw_r_instruction,
	output wire is_p_mulu_instruction,
	output wire is_p_mulhhu_instruction,
	output wire is_p_mulun_instruction,
	output wire is_p_mulhhun_instruction,
	output wire is_p_mulurn_instruction,
	output wire is_p_mulhhurn_instruction,
	output wire is_p_macsn_instruction,
	output wire is_p_machhsn_instruction,
	output wire is_p_macsrn_instruction,
	output wire is_p_machhsrn_instruction,
	output wire is_p_macun_instruction,
	output wire is_p_machhun_instruction,
	output wire is_p_macurn_instruction,
	output wire is_p_machhurn_instruction,
	output wire is_p_sb_i_inc_instruction,
	output wire is_p_sh_i_inc_instruction,
	output wire is_p_sw_i_inc_instruction,
	output wire is_p_sb_r_inc_instruction,
	output wire is_p_sh_r_inc_instruction,
	output wire is_p_sw_r_inc_instruction,
	output wire is_p_sb_r_instruction,
	output wire is_p_sh_r_instruction,
	output wire is_p_sw_r_instruction,
	output wire is_pv_cmpeq_h_instruction,
	output wire is_pv_cmpeq_sc_h_instruction,
	output wire is_pv_cmpeq_sci_h_instruction,
	output wire is_pv_cmpeq_b_instruction,
	output wire is_pv_cmpeq_sc_b_instruction,
	output wire is_pv_cmpeq_sci_b_instruction,
	output wire is_pv_cmpne_h_instruction,
	output wire is_pv_cmpne_sc_h_instruction,
	output wire is_pv_cmpne_sci_h_instruction,
	output wire is_pv_cmpne_b_instruction,
	output wire is_pv_cmpne_sc_b_instruction,
	output wire is_pv_cmpne_sci_b_instruction,
	output wire is_pv_cmpgt_h_instruction,
	output wire is_pv_cmpgt_sc_h_instruction,
	output wire is_pv_cmpgt_sci_h_instruction,
	output wire is_pv_cmpgt_b_instruction,
	output wire is_pv_cmpgt_sc_b_instruction,
	output wire is_pv_cmpgt_sci_b_instruction,
	output wire is_pv_cmpge_h_instruction,
	output wire is_pv_cmpge_sc_h_instruction,
	output wire is_pv_cmpge_sci_h_instruction,
	output wire is_pv_cmpge_b_instruction,
	output wire is_pv_cmpge_sc_b_instruction,
	output wire is_pv_cmpge_sci_b_instruction,
	output wire is_pv_cmplt_h_instruction,
	output wire is_pv_cmplt_sc_h_instruction,
	output wire is_pv_cmplt_sci_h_instruction,
	output wire is_pv_cmplt_b_instruction,
	output wire is_pv_cmplt_sc_b_instruction,
	output wire is_pv_cmplt_sci_b_instruction,
	output wire is_pv_cmple_h_instruction,
	output wire is_pv_cmple_sc_h_instruction,
	output wire is_pv_cmple_sci_h_instruction,
	output wire is_pv_cmple_b_instruction,
	output wire is_pv_cmple_sc_b_instruction,
	output wire is_pv_cmple_sci_b_instruction,
	output wire is_pv_cmpgtu_h_instruction,
	output wire is_pv_cmpgtu_sc_h_instruction,
	output wire is_pv_cmpgtu_sci_h_instruction,
	output wire is_pv_cmpgtu_b_instruction,
	output wire is_pv_cmpgtu_sc_b_instruction,
	output wire is_pv_cmpgtu_sci_b_instruction,
	output wire is_pv_cmpgeu_h_instruction,
	output wire is_pv_cmpgeu_sc_h_instruction,
	output wire is_pv_cmpgeu_sci_h_instruction,
	output wire is_pv_cmpgeu_b_instruction,
	output wire is_pv_cmpgeu_sc_b_instruction,
	output wire is_pv_cmpgeu_sci_b_instruction,
	output wire is_pv_cmpltu_h_instruction,
	output wire is_pv_cmpltu_sc_h_instruction,
	output wire is_pv_cmpltu_sci_h_instruction,
	output wire is_pv_cmpltu_b_instruction,
	output wire is_pv_cmpltu_sc_b_instruction,
	output wire is_pv_cmpltu_sci_b_instruction,
	output wire is_pv_cmpleu_h_instruction,
	output wire is_pv_cmpleu_sc_h_instruction,
	output wire is_pv_cmpleu_sci_h_instruction,
	output wire is_pv_cmpleu_b_instruction,
	output wire is_pv_cmpleu_sc_b_instruction,
	output wire is_pv_cmpleu_sci_b_instruction,
	output wire is_pv_add_h_instruction,
	output wire is_pv_add_sc_h_instruction,
	output wire is_pv_add_sci_h_instruction,
	output wire is_pv_add_b_instruction,
	output wire is_pv_add_sc_b_instruction,
	output wire is_pv_add_sci_b_instruction,
	output wire is_pv_sub_h_instruction,
	output wire is_pv_sub_sc_h_instruction,
	output wire is_pv_sub_sci_h_instruction,
	output wire is_pv_sub_b_instruction,
	output wire is_pv_sub_sc_b_instruction,
	output wire is_pv_sub_sci_b_instruction,
	output wire is_pv_avg_h_instruction,
	output wire is_pv_avg_sc_h_instruction,
	output wire is_pv_avg_sci_h_instruction,
	output wire is_pv_avg_b_instruction,
	output wire is_pv_avg_sc_b_instruction,
	output wire is_pv_avg_sci_b_instruction,
	output wire is_pv_avgu_h_instruction,
	output wire is_pv_avgu_sc_h_instruction,
	output wire is_pv_avgu_sci_h_instruction,
	output wire is_pv_avgu_b_instruction,
	output wire is_pv_avgu_sc_b_instruction,
	output wire is_pv_avgu_sci_b_instruction,
	output wire is_pv_min_h_instruction,
	output wire is_pv_min_sc_h_instruction,
	output wire is_pv_min_sci_h_instruction,
	output wire is_pv_min_b_instruction,
	output wire is_pv_min_sc_b_instruction,
	output wire is_pv_min_sci_b_instruction,
	output wire is_pv_minu_h_instruction,
	output wire is_pv_minu_sc_h_instruction,
	output wire is_pv_minu_sci_h_instruction,
	output wire is_pv_minu_b_instruction,
	output wire is_pv_minu_sc_b_instruction,
	output wire is_pv_minu_sci_b_instruction,
	output wire is_pv_max_h_instruction,
	output wire is_pv_max_sc_h_instruction,
	output wire is_pv_max_sci_h_instruction,
	output wire is_pv_max_b_instruction,
	output wire is_pv_max_sc_b_instruction,
	output wire is_pv_max_sci_b_instruction,
	output wire is_pv_maxu_h_instruction,
	output wire is_pv_maxu_sc_h_instruction,
	output wire is_pv_maxu_sci_h_instruction,
	output wire is_pv_maxu_b_instruction,
	output wire is_pv_maxu_sc_b_instruction,
	output wire is_pv_maxu_sci_b_instruction,
	output wire is_pv_srl_h_instruction,
	output wire is_pv_srl_sc_h_instruction,
	output wire is_pv_srl_sci_h_instruction,
	output wire is_pv_srl_b_instruction,
	output wire is_pv_srl_sc_b_instruction,
	output wire is_pv_srl_sci_b_instruction,
	output wire is_pv_sra_h_instruction,
	output wire is_pv_sra_sc_h_instruction,
	output wire is_pv_sra_sci_h_instruction,
	output wire is_pv_sra_b_instruction,
	output wire is_pv_sra_sc_b_instruction,
	output wire is_pv_sra_sci_b_instruction,
	output wire is_pv_sll_h_instruction,
	output wire is_pv_sll_sc_h_instruction,
	output wire is_pv_sll_sci_h_instruction,
	output wire is_pv_sll_b_instruction,
	output wire is_pv_sll_sc_b_instruction,
	output wire is_pv_sll_sci_b_instruction,
	output wire is_pv_or_h_instruction,
	output wire is_pv_or_sc_h_instruction,
	output wire is_pv_or_sci_h_instruction,
	output wire is_pv_or_b_instruction,
	output wire is_pv_or_sc_b_instruction,
	output wire is_pv_or_sci_b_instruction,
	output wire is_pv_xor_h_instruction,
	output wire is_pv_xor_sc_h_instruction,
	output wire is_pv_xor_sci_h_instruction,
	output wire is_pv_xor_b_instruction,
	output wire is_pv_xor_sc_b_instruction,
	output wire is_pv_xor_sci_b_instruction,
	output wire is_pv_and_h_instruction,
	output wire is_pv_and_sc_h_instruction,
	output wire is_pv_and_sci_h_instruction,
	output wire is_pv_and_b_instruction,
	output wire is_pv_and_sc_b_instruction,
	output wire is_pv_and_sci_b_instruction,
	output wire is_pv_abs_h_instruction,
	output wire is_pv_abs_b_instruction,
	output wire is_pv_extract_h_instruction,
	output wire is_pv_extract_b_instruction,
	output wire is_pv_extractu_h_instruction,
	output wire is_pv_extractu_b_instruction,
	output wire is_pv_insert_h_instruction,
	output wire is_pv_insert_b_instruction,
	output wire is_pv_dotup_h_instruction,
	output wire is_pv_dotup_sc_h_instruction,
	output wire is_pv_dotup_sci_h_instruction,
	output wire is_pv_dotup_b_instruction,
	output wire is_pv_dotup_sc_b_instruction,
	output wire is_pv_dotup_sci_b_instruction,
	output wire is_pv_dotusp_h_instruction,
	output wire is_pv_dotusp_sc_h_instruction,
	output wire is_pv_dotusp_sci_h_instruction,
	output wire is_pv_dotusp_b_instruction,
	output wire is_pv_dotusp_sc_b_instruction,
	output wire is_pv_dotusp_sci_b_instruction,
	output wire is_pv_dotsp_h_instruction,
	output wire is_pv_dotsp_sc_h_instruction,
	output wire is_pv_dotsp_sci_h_instruction,
	output wire is_pv_dotsp_b_instruction,
	output wire is_pv_dotsp_sc_b_instruction,
	output wire is_pv_dotsp_sci_b_instruction,
	output wire is_pv_sdotup_h_instruction,
	output wire is_pv_sdotup_sc_h_instruction,
	output wire is_pv_sdotup_sci_h_instruction,
	output wire is_pv_sdotup_b_instruction,
	output wire is_pv_sdotup_sc_b_instruction,
	output wire is_pv_sdotup_sci_b_instruction,
	output wire is_pv_sdotusp_h_instruction,
	output wire is_pv_sdotusp_sc_h_instruction,
	output wire is_pv_sdotusp_sci_h_instruction,
	output wire is_pv_sdotusp_b_instruction,
	output wire is_pv_sdotusp_sc_b_instruction,
	output wire is_pv_sdotusp_sci_b_instruction,
	output wire is_pv_sdotsp_h_instruction,
	output wire is_pv_sdotsp_sc_h_instruction,
	output wire is_pv_sdotsp_sci_h_instruction,
	output wire is_pv_sdotsp_b_instruction,
	output wire is_pv_sdotsp_sc_b_instruction,
	output wire is_pv_sdotsp_sci_b_instruction,
	output wire is_pv_shuffle_h_instruction,
	output wire is_pv_shuffle_sci_h_instruction,
	output wire is_pv_shuffle_b_instruction,
	output wire is_pv_shufflei0_sci_b_instruction,
	output wire is_pv_shufflei1_sci_b_instruction,
	output wire is_pv_shufflei2_sci_b_instruction,
	output wire is_pv_shufflei3_sci_b_instruction,
	output wire is_pv_shuffle2_h_instruction,
	output wire is_pv_shuffle2_b_instruction,
	output wire is_pv_pack_h_instruction,
	output wire is_pv_packhi_b_instruction,
	output wire is_pv_packlo_b_instruction,
	output wire has_operand_L,
	output wire has_operand_csr,
	output wire has_operand_fm,
	output wire has_operand_imm,
	output wire has_operand_imm1,
	output wire has_operand_imm2,
	output wire has_operand_ls2,
	output wire has_operand_ls3,
	output wire has_operand_pred,
	output wire has_operand_rd,
	output wire has_operand_rs1,
	output wire has_operand_rs2,
	output wire has_operand_shamt,
	output wire has_operand_succ,
	output wire has_operand_uimm,
	output wire has_operand_uimmL,
	output wire has_operand_uimmS,
	output wire has_operand_zimm,
	output wire [4:0] operand_rd,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rs2,
	output wire [11:0] operand_csr,
	output wire [0:0] operand_L_lp_starti,
	output wire [0:0] operand_L_lp_endi,
	output wire [0:0] operand_L_lp_counti,
	output wire [0:0] operand_L_lp_count,
	output wire [0:0] operand_L_lp_setup,
	output wire [3:0] operand_fm_fence,
	output wire [31:12] operand_imm_lui,
	output wire [31:12] operand_imm_auipc,
	output wire [20:1] operand_imm_jal,
	output wire [11:0] operand_imm_jalr,
	output wire [12:1] operand_imm_beq,
	output wire [12:1] operand_imm_bne,
	output wire [12:1] operand_imm_blt,
	output wire [12:1] operand_imm_bge,
	output wire [12:1] operand_imm_bltu,
	output wire [12:1] operand_imm_bgeu,
	output wire [11:0] operand_imm_lb,
	output wire [11:0] operand_imm_lh,
	output wire [11:0] operand_imm_lw,
	output wire [11:0] operand_imm_lbu,
	output wire [11:0] operand_imm_lhu,
	output wire [11:0] operand_imm_sb,
	output wire [11:0] operand_imm_sh,
	output wire [11:0] operand_imm_sw,
	output wire [11:0] operand_imm_addi,
	output wire [11:0] operand_imm_slti,
	output wire [11:0] operand_imm_sltiu,
	output wire [11:0] operand_imm_xori,
	output wire [11:0] operand_imm_ori,
	output wire [11:0] operand_imm_andi,
	output wire [11:0] operand_imm_p_lb_i_inc,
	output wire [11:0] operand_imm_p_lbu_i_inc,
	output wire [11:0] operand_imm_p_lh_i_inc,
	output wire [11:0] operand_imm_p_lhu_i_inc,
	output wire [11:0] operand_imm_p_lw_i_inc,
	output wire [11:0] operand_imm_p_sb_i_inc,
	output wire [11:0] operand_imm_p_sh_i_inc,
	output wire [11:0] operand_imm_p_sw_i_inc,
	output wire [5:0] operand_imm_pv_cmpeq_sci_h,
	output wire [5:0] operand_imm_pv_cmpeq_sci_b,
	output wire [5:0] operand_imm_pv_cmpne_sci_h,
	output wire [5:0] operand_imm_pv_cmpne_sci_b,
	output wire [5:0] operand_imm_pv_cmpgt_sci_h,
	output wire [5:0] operand_imm_pv_cmpgt_sci_b,
	output wire [5:0] operand_imm_pv_cmpge_sci_h,
	output wire [5:0] operand_imm_pv_cmpge_sci_b,
	output wire [5:0] operand_imm_pv_cmplt_sci_h,
	output wire [5:0] operand_imm_pv_cmplt_sci_b,
	output wire [5:0] operand_imm_pv_cmple_sci_h,
	output wire [5:0] operand_imm_pv_cmple_sci_b,
	output wire [5:0] operand_imm_pv_cmpgtu_sci_h,
	output wire [5:0] operand_imm_pv_cmpgtu_sci_b,
	output wire [5:0] operand_imm_pv_cmpgeu_sci_h,
	output wire [5:0] operand_imm_pv_cmpgeu_sci_b,
	output wire [5:0] operand_imm_pv_cmpltu_sci_h,
	output wire [5:0] operand_imm_pv_cmpltu_sci_b,
	output wire [5:0] operand_imm_pv_cmpleu_sci_h,
	output wire [5:0] operand_imm_pv_cmpleu_sci_b,
	output wire [5:0] operand_imm_pv_add_sci_h,
	output wire [5:0] operand_imm_pv_add_sci_b,
	output wire [5:0] operand_imm_pv_sub_sci_h,
	output wire [5:0] operand_imm_pv_sub_sci_b,
	output wire [5:0] operand_imm_pv_avg_sci_h,
	output wire [5:0] operand_imm_pv_avg_sci_b,
	output wire [5:0] operand_imm_pv_avgu_sci_h,
	output wire [5:0] operand_imm_pv_avgu_sci_b,
	output wire [5:0] operand_imm_pv_min_sci_h,
	output wire [5:0] operand_imm_pv_min_sci_b,
	output wire [5:0] operand_imm_pv_minu_sci_h,
	output wire [5:0] operand_imm_pv_minu_sci_b,
	output wire [5:0] operand_imm_pv_max_sci_h,
	output wire [5:0] operand_imm_pv_max_sci_b,
	output wire [5:0] operand_imm_pv_maxu_sci_h,
	output wire [5:0] operand_imm_pv_maxu_sci_b,
	output wire [5:0] operand_imm_pv_srl_sci_h,
	output wire [5:0] operand_imm_pv_srl_sci_b,
	output wire [5:0] operand_imm_pv_sra_sci_h,
	output wire [5:0] operand_imm_pv_sra_sci_b,
	output wire [5:0] operand_imm_pv_sll_sci_h,
	output wire [5:0] operand_imm_pv_sll_sci_b,
	output wire [5:0] operand_imm_pv_or_sci_h,
	output wire [5:0] operand_imm_pv_or_sci_b,
	output wire [5:0] operand_imm_pv_xor_sci_h,
	output wire [5:0] operand_imm_pv_xor_sci_b,
	output wire [5:0] operand_imm_pv_and_sci_h,
	output wire [5:0] operand_imm_pv_and_sci_b,
	output wire [5:0] operand_imm_pv_extract_h,
	output wire [5:0] operand_imm_pv_extract_b,
	output wire [5:0] operand_imm_pv_extractu_h,
	output wire [5:0] operand_imm_pv_extractu_b,
	output wire [5:0] operand_imm_pv_insert_h,
	output wire [5:0] operand_imm_pv_insert_b,
	output wire [5:0] operand_imm_pv_dotup_sci_h,
	output wire [5:0] operand_imm_pv_dotup_sci_b,
	output wire [5:0] operand_imm_pv_dotusp_sci_h,
	output wire [5:0] operand_imm_pv_dotusp_sci_b,
	output wire [5:0] operand_imm_pv_dotsp_sci_h,
	output wire [5:0] operand_imm_pv_dotsp_sci_b,
	output wire [5:0] operand_imm_pv_sdotup_sci_h,
	output wire [5:0] operand_imm_pv_sdotup_sci_b,
	output wire [5:0] operand_imm_pv_sdotusp_sci_h,
	output wire [5:0] operand_imm_pv_sdotusp_sci_b,
	output wire [5:0] operand_imm_pv_sdotsp_sci_h,
	output wire [5:0] operand_imm_pv_sdotsp_sci_b,
	output wire [5:0] operand_imm_pv_shuffle_sci_h,
	output wire [5:0] operand_imm_pv_shufflei0_sci_b,
	output wire [5:0] operand_imm_pv_shufflei1_sci_b,
	output wire [5:0] operand_imm_pv_shufflei2_sci_b,
	output wire [5:0] operand_imm_pv_shufflei3_sci_b,
	output wire [12:1] operand_imm1_p_beqimm,
	output wire [12:1] operand_imm1_p_bneimm,
	output wire [4:0] operand_imm2_p_beqimm,
	output wire [4:0] operand_imm2_p_bneimm,
	output wire [4:0] operand_ls2_p_extract,
	output wire [4:0] operand_ls2_p_extractu,
	output wire [4:0] operand_ls2_p_insert,
	output wire [4:0] operand_ls2_p_bclr,
	output wire [4:0] operand_ls2_p_bset,
	output wire [4:0] operand_ls3_p_extract,
	output wire [4:0] operand_ls3_p_extractu,
	output wire [4:0] operand_ls3_p_insert,
	output wire [4:0] operand_ls3_p_bclr,
	output wire [4:0] operand_ls3_p_bset,
	output wire [3:0] operand_pred_fence,
	output wire [4:0] operand_shamt_slli,
	output wire [4:0] operand_shamt_srli,
	output wire [4:0] operand_shamt_srai,
	output wire [3:0] operand_succ_fence,
	output wire [4:0] operand_uimm_p_clip,
	output wire [4:0] operand_uimm_p_clipu,
	output wire [4:0] operand_uimm_p_addn,
	output wire [4:0] operand_uimm_p_addun,
	output wire [4:0] operand_uimm_p_addrn,
	output wire [4:0] operand_uimm_p_addurn,
	output wire [4:0] operand_uimm_p_subn,
	output wire [4:0] operand_uimm_p_subun,
	output wire [4:0] operand_uimm_p_subrn,
	output wire [4:0] operand_uimm_p_suburn,
	output wire [4:0] operand_uimm_p_addnr,
	output wire [4:0] operand_uimm_p_mulun,
	output wire [4:0] operand_uimm_p_mulhhun,
	output wire [4:0] operand_uimm_p_mulurn,
	output wire [4:0] operand_uimm_p_mulhhurn,
	output wire [4:0] operand_uimm_p_macsn,
	output wire [4:0] operand_uimm_p_machhsn,
	output wire [4:0] operand_uimm_p_macsrn,
	output wire [4:0] operand_uimm_p_machhsrn,
	output wire [4:0] operand_uimm_p_macun,
	output wire [4:0] operand_uimm_p_machhun,
	output wire [4:0] operand_uimm_p_macurn,
	output wire [4:0] operand_uimm_p_machhurn,
	output wire [11:0] operand_uimmL_lp_starti,
	output wire [11:0] operand_uimmL_lp_endi,
	output wire [11:0] operand_uimmL_lp_counti,
	output wire [11:0] operand_uimmL_lp_setup,
	output wire [11:0] operand_uimmL_lp_setupi,
	output wire [4:0] operand_uimmS_lp_setupi,
	output wire [4:0] operand_zimm_csrrwi,
	output wire [4:0] operand_zimm_csrrsi,
	output wire [4:0] operand_zimm_csrrci,

	output wire just_ignore_me

);
	wire [4:0] operand_rd_lui;
	wire [4:0] operand_rd_auipc;
	wire [4:0] operand_rd_jal;
	wire [4:0] operand_rd_jalr;
	wire [4:0] operand_rd_lb;
	wire [4:0] operand_rd_lh;
	wire [4:0] operand_rd_lw;
	wire [4:0] operand_rd_lbu;
	wire [4:0] operand_rd_lhu;
	wire [4:0] operand_rd_addi;
	wire [4:0] operand_rd_slti;
	wire [4:0] operand_rd_sltiu;
	wire [4:0] operand_rd_xori;
	wire [4:0] operand_rd_ori;
	wire [4:0] operand_rd_andi;
	wire [4:0] operand_rd_slli;
	wire [4:0] operand_rd_srli;
	wire [4:0] operand_rd_srai;
	wire [4:0] operand_rd_add;
	wire [4:0] operand_rd_sub;
	wire [4:0] operand_rd_sll;
	wire [4:0] operand_rd_slt;
	wire [4:0] operand_rd_sltu;
	wire [4:0] operand_rd_xor;
	wire [4:0] operand_rd_srl;
	wire [4:0] operand_rd_sra;
	wire [4:0] operand_rd_or;
	wire [4:0] operand_rd_and;
	wire [4:0] operand_rd_fence;
	wire [4:0] operand_rd_mul;
	wire [4:0] operand_rd_mulh;
	wire [4:0] operand_rd_mulhsu;
	wire [4:0] operand_rd_mulhu;
	wire [4:0] operand_rd_div;
	wire [4:0] operand_rd_divu;
	wire [4:0] operand_rd_rem;
	wire [4:0] operand_rd_remu;
	wire [4:0] operand_rd_csrrw;
	wire [4:0] operand_rd_csrrs;
	wire [4:0] operand_rd_csrrc;
	wire [4:0] operand_rd_csrrwi;
	wire [4:0] operand_rd_csrrsi;
	wire [4:0] operand_rd_csrrci;
	wire [4:0] operand_rd_p_extract;
	wire [4:0] operand_rd_p_extractu;
	wire [4:0] operand_rd_p_insert;
	wire [4:0] operand_rd_p_bclr;
	wire [4:0] operand_rd_p_bset;
	wire [4:0] operand_rd_p_extractr;
	wire [4:0] operand_rd_p_extractur;
	wire [4:0] operand_rd_p_insertr;
	wire [4:0] operand_rd_p_bclrr;
	wire [4:0] operand_rd_p_bsetr;
	wire [4:0] operand_rd_p_ror;
	wire [4:0] operand_rd_p_ff1;
	wire [4:0] operand_rd_p_fl1;
	wire [4:0] operand_rd_p_clb;
	wire [4:0] operand_rd_p_cnt;
	wire [4:0] operand_rd_p_abs;
	wire [4:0] operand_rd_p_slet;
	wire [4:0] operand_rd_p_sletu;
	wire [4:0] operand_rd_p_min;
	wire [4:0] operand_rd_p_minu;
	wire [4:0] operand_rd_p_max;
	wire [4:0] operand_rd_p_maxu;
	wire [4:0] operand_rd_p_exths;
	wire [4:0] operand_rd_p_exthz;
	wire [4:0] operand_rd_p_extbs;
	wire [4:0] operand_rd_p_extbz;
	wire [4:0] operand_rd_p_clip;
	wire [4:0] operand_rd_p_clipu;
	wire [4:0] operand_rd_p_clipr;
	wire [4:0] operand_rd_p_clipur;
	wire [4:0] operand_rd_p_addn;
	wire [4:0] operand_rd_p_addun;
	wire [4:0] operand_rd_p_addrn;
	wire [4:0] operand_rd_p_addurn;
	wire [4:0] operand_rd_p_subn;
	wire [4:0] operand_rd_p_subun;
	wire [4:0] operand_rd_p_subrn;
	wire [4:0] operand_rd_p_suburn;
	wire [4:0] operand_rd_p_addnr;
	wire [4:0] operand_rd_p_addunr;
	wire [4:0] operand_rd_p_addrnr;
	wire [4:0] operand_rd_p_addurnr;
	wire [4:0] operand_rd_p_subnr;
	wire [4:0] operand_rd_p_subunr;
	wire [4:0] operand_rd_p_subrnr;
	wire [4:0] operand_rd_p_suburnr;
	wire [4:0] operand_rd_p_lb_i_inc;
	wire [4:0] operand_rd_p_lbu_i_inc;
	wire [4:0] operand_rd_p_lh_i_inc;
	wire [4:0] operand_rd_p_lhu_i_inc;
	wire [4:0] operand_rd_p_lw_i_inc;
	wire [4:0] operand_rd_p_lb_r_inc;
	wire [4:0] operand_rd_p_lbu_r_inc;
	wire [4:0] operand_rd_p_lh_r_inc;
	wire [4:0] operand_rd_p_lhu_r_inc;
	wire [4:0] operand_rd_p_lw_r_inc;
	wire [4:0] operand_rd_p_lb_r;
	wire [4:0] operand_rd_p_lbu_r;
	wire [4:0] operand_rd_p_lh_r;
	wire [4:0] operand_rd_p_lhu_r;
	wire [4:0] operand_rd_p_lw_r;
	wire [4:0] operand_rd_p_mulu;
	wire [4:0] operand_rd_p_mulhhu;
	wire [4:0] operand_rd_p_mulun;
	wire [4:0] operand_rd_p_mulhhun;
	wire [4:0] operand_rd_p_mulurn;
	wire [4:0] operand_rd_p_mulhhurn;
	wire [4:0] operand_rd_p_macsn;
	wire [4:0] operand_rd_p_machhsn;
	wire [4:0] operand_rd_p_macsrn;
	wire [4:0] operand_rd_p_machhsrn;
	wire [4:0] operand_rd_p_macun;
	wire [4:0] operand_rd_p_machhun;
	wire [4:0] operand_rd_p_macurn;
	wire [4:0] operand_rd_p_machhurn;
	wire [4:0] operand_rd_p_sb_r_inc;
	wire [4:0] operand_rd_p_sh_r_inc;
	wire [4:0] operand_rd_p_sw_r_inc;
	wire [4:0] operand_rd_p_sb_r;
	wire [4:0] operand_rd_p_sh_r;
	wire [4:0] operand_rd_p_sw_r;
	wire [4:0] operand_rd_pv_cmpeq_h;
	wire [4:0] operand_rd_pv_cmpeq_sc_h;
	wire [4:0] operand_rd_pv_cmpeq_sci_h;
	wire [4:0] operand_rd_pv_cmpeq_b;
	wire [4:0] operand_rd_pv_cmpeq_sc_b;
	wire [4:0] operand_rd_pv_cmpeq_sci_b;
	wire [4:0] operand_rd_pv_cmpne_h;
	wire [4:0] operand_rd_pv_cmpne_sc_h;
	wire [4:0] operand_rd_pv_cmpne_sci_h;
	wire [4:0] operand_rd_pv_cmpne_b;
	wire [4:0] operand_rd_pv_cmpne_sc_b;
	wire [4:0] operand_rd_pv_cmpne_sci_b;
	wire [4:0] operand_rd_pv_cmpgt_h;
	wire [4:0] operand_rd_pv_cmpgt_sc_h;
	wire [4:0] operand_rd_pv_cmpgt_sci_h;
	wire [4:0] operand_rd_pv_cmpgt_b;
	wire [4:0] operand_rd_pv_cmpgt_sc_b;
	wire [4:0] operand_rd_pv_cmpgt_sci_b;
	wire [4:0] operand_rd_pv_cmpge_h;
	wire [4:0] operand_rd_pv_cmpge_sc_h;
	wire [4:0] operand_rd_pv_cmpge_sci_h;
	wire [4:0] operand_rd_pv_cmpge_b;
	wire [4:0] operand_rd_pv_cmpge_sc_b;
	wire [4:0] operand_rd_pv_cmpge_sci_b;
	wire [4:0] operand_rd_pv_cmplt_h;
	wire [4:0] operand_rd_pv_cmplt_sc_h;
	wire [4:0] operand_rd_pv_cmplt_sci_h;
	wire [4:0] operand_rd_pv_cmplt_b;
	wire [4:0] operand_rd_pv_cmplt_sc_b;
	wire [4:0] operand_rd_pv_cmplt_sci_b;
	wire [4:0] operand_rd_pv_cmple_h;
	wire [4:0] operand_rd_pv_cmple_sc_h;
	wire [4:0] operand_rd_pv_cmple_sci_h;
	wire [4:0] operand_rd_pv_cmple_b;
	wire [4:0] operand_rd_pv_cmple_sc_b;
	wire [4:0] operand_rd_pv_cmple_sci_b;
	wire [4:0] operand_rd_pv_cmpgtu_h;
	wire [4:0] operand_rd_pv_cmpgtu_sc_h;
	wire [4:0] operand_rd_pv_cmpgtu_sci_h;
	wire [4:0] operand_rd_pv_cmpgtu_b;
	wire [4:0] operand_rd_pv_cmpgtu_sc_b;
	wire [4:0] operand_rd_pv_cmpgtu_sci_b;
	wire [4:0] operand_rd_pv_cmpgeu_h;
	wire [4:0] operand_rd_pv_cmpgeu_sc_h;
	wire [4:0] operand_rd_pv_cmpgeu_sci_h;
	wire [4:0] operand_rd_pv_cmpgeu_b;
	wire [4:0] operand_rd_pv_cmpgeu_sc_b;
	wire [4:0] operand_rd_pv_cmpgeu_sci_b;
	wire [4:0] operand_rd_pv_cmpltu_h;
	wire [4:0] operand_rd_pv_cmpltu_sc_h;
	wire [4:0] operand_rd_pv_cmpltu_sci_h;
	wire [4:0] operand_rd_pv_cmpltu_b;
	wire [4:0] operand_rd_pv_cmpltu_sc_b;
	wire [4:0] operand_rd_pv_cmpltu_sci_b;
	wire [4:0] operand_rd_pv_cmpleu_h;
	wire [4:0] operand_rd_pv_cmpleu_sc_h;
	wire [4:0] operand_rd_pv_cmpleu_sci_h;
	wire [4:0] operand_rd_pv_cmpleu_b;
	wire [4:0] operand_rd_pv_cmpleu_sc_b;
	wire [4:0] operand_rd_pv_cmpleu_sci_b;
	wire [4:0] operand_rd_pv_add_h;
	wire [4:0] operand_rd_pv_add_sc_h;
	wire [4:0] operand_rd_pv_add_sci_h;
	wire [4:0] operand_rd_pv_add_b;
	wire [4:0] operand_rd_pv_add_sc_b;
	wire [4:0] operand_rd_pv_add_sci_b;
	wire [4:0] operand_rd_pv_sub_h;
	wire [4:0] operand_rd_pv_sub_sc_h;
	wire [4:0] operand_rd_pv_sub_sci_h;
	wire [4:0] operand_rd_pv_sub_b;
	wire [4:0] operand_rd_pv_sub_sc_b;
	wire [4:0] operand_rd_pv_sub_sci_b;
	wire [4:0] operand_rd_pv_avg_h;
	wire [4:0] operand_rd_pv_avg_sc_h;
	wire [4:0] operand_rd_pv_avg_sci_h;
	wire [4:0] operand_rd_pv_avg_b;
	wire [4:0] operand_rd_pv_avg_sc_b;
	wire [4:0] operand_rd_pv_avg_sci_b;
	wire [4:0] operand_rd_pv_avgu_h;
	wire [4:0] operand_rd_pv_avgu_sc_h;
	wire [4:0] operand_rd_pv_avgu_sci_h;
	wire [4:0] operand_rd_pv_avgu_b;
	wire [4:0] operand_rd_pv_avgu_sc_b;
	wire [4:0] operand_rd_pv_avgu_sci_b;
	wire [4:0] operand_rd_pv_min_h;
	wire [4:0] operand_rd_pv_min_sc_h;
	wire [4:0] operand_rd_pv_min_sci_h;
	wire [4:0] operand_rd_pv_min_b;
	wire [4:0] operand_rd_pv_min_sc_b;
	wire [4:0] operand_rd_pv_min_sci_b;
	wire [4:0] operand_rd_pv_minu_h;
	wire [4:0] operand_rd_pv_minu_sc_h;
	wire [4:0] operand_rd_pv_minu_sci_h;
	wire [4:0] operand_rd_pv_minu_b;
	wire [4:0] operand_rd_pv_minu_sc_b;
	wire [4:0] operand_rd_pv_minu_sci_b;
	wire [4:0] operand_rd_pv_max_h;
	wire [4:0] operand_rd_pv_max_sc_h;
	wire [4:0] operand_rd_pv_max_sci_h;
	wire [4:0] operand_rd_pv_max_b;
	wire [4:0] operand_rd_pv_max_sc_b;
	wire [4:0] operand_rd_pv_max_sci_b;
	wire [4:0] operand_rd_pv_maxu_h;
	wire [4:0] operand_rd_pv_maxu_sc_h;
	wire [4:0] operand_rd_pv_maxu_sci_h;
	wire [4:0] operand_rd_pv_maxu_b;
	wire [4:0] operand_rd_pv_maxu_sc_b;
	wire [4:0] operand_rd_pv_maxu_sci_b;
	wire [4:0] operand_rd_pv_srl_h;
	wire [4:0] operand_rd_pv_srl_sc_h;
	wire [4:0] operand_rd_pv_srl_sci_h;
	wire [4:0] operand_rd_pv_srl_b;
	wire [4:0] operand_rd_pv_srl_sc_b;
	wire [4:0] operand_rd_pv_srl_sci_b;
	wire [4:0] operand_rd_pv_sra_h;
	wire [4:0] operand_rd_pv_sra_sc_h;
	wire [4:0] operand_rd_pv_sra_sci_h;
	wire [4:0] operand_rd_pv_sra_b;
	wire [4:0] operand_rd_pv_sra_sc_b;
	wire [4:0] operand_rd_pv_sra_sci_b;
	wire [4:0] operand_rd_pv_sll_h;
	wire [4:0] operand_rd_pv_sll_sc_h;
	wire [4:0] operand_rd_pv_sll_sci_h;
	wire [4:0] operand_rd_pv_sll_b;
	wire [4:0] operand_rd_pv_sll_sc_b;
	wire [4:0] operand_rd_pv_sll_sci_b;
	wire [4:0] operand_rd_pv_or_h;
	wire [4:0] operand_rd_pv_or_sc_h;
	wire [4:0] operand_rd_pv_or_sci_h;
	wire [4:0] operand_rd_pv_or_b;
	wire [4:0] operand_rd_pv_or_sc_b;
	wire [4:0] operand_rd_pv_or_sci_b;
	wire [4:0] operand_rd_pv_xor_h;
	wire [4:0] operand_rd_pv_xor_sc_h;
	wire [4:0] operand_rd_pv_xor_sci_h;
	wire [4:0] operand_rd_pv_xor_b;
	wire [4:0] operand_rd_pv_xor_sc_b;
	wire [4:0] operand_rd_pv_xor_sci_b;
	wire [4:0] operand_rd_pv_and_h;
	wire [4:0] operand_rd_pv_and_sc_h;
	wire [4:0] operand_rd_pv_and_sci_h;
	wire [4:0] operand_rd_pv_and_b;
	wire [4:0] operand_rd_pv_and_sc_b;
	wire [4:0] operand_rd_pv_and_sci_b;
	wire [4:0] operand_rd_pv_abs_h;
	wire [4:0] operand_rd_pv_abs_b;
	wire [4:0] operand_rd_pv_extract_h;
	wire [4:0] operand_rd_pv_extract_b;
	wire [4:0] operand_rd_pv_extractu_h;
	wire [4:0] operand_rd_pv_extractu_b;
	wire [4:0] operand_rd_pv_insert_h;
	wire [4:0] operand_rd_pv_insert_b;
	wire [4:0] operand_rd_pv_dotup_h;
	wire [4:0] operand_rd_pv_dotup_sc_h;
	wire [4:0] operand_rd_pv_dotup_sci_h;
	wire [4:0] operand_rd_pv_dotup_b;
	wire [4:0] operand_rd_pv_dotup_sc_b;
	wire [4:0] operand_rd_pv_dotup_sci_b;
	wire [4:0] operand_rd_pv_dotusp_h;
	wire [4:0] operand_rd_pv_dotusp_sc_h;
	wire [4:0] operand_rd_pv_dotusp_sci_h;
	wire [4:0] operand_rd_pv_dotusp_b;
	wire [4:0] operand_rd_pv_dotusp_sc_b;
	wire [4:0] operand_rd_pv_dotusp_sci_b;
	wire [4:0] operand_rd_pv_dotsp_h;
	wire [4:0] operand_rd_pv_dotsp_sc_h;
	wire [4:0] operand_rd_pv_dotsp_sci_h;
	wire [4:0] operand_rd_pv_dotsp_b;
	wire [4:0] operand_rd_pv_dotsp_sc_b;
	wire [4:0] operand_rd_pv_dotsp_sci_b;
	wire [4:0] operand_rd_pv_sdotup_h;
	wire [4:0] operand_rd_pv_sdotup_sc_h;
	wire [4:0] operand_rd_pv_sdotup_sci_h;
	wire [4:0] operand_rd_pv_sdotup_b;
	wire [4:0] operand_rd_pv_sdotup_sc_b;
	wire [4:0] operand_rd_pv_sdotup_sci_b;
	wire [4:0] operand_rd_pv_sdotusp_h;
	wire [4:0] operand_rd_pv_sdotusp_sc_h;
	wire [4:0] operand_rd_pv_sdotusp_sci_h;
	wire [4:0] operand_rd_pv_sdotusp_b;
	wire [4:0] operand_rd_pv_sdotusp_sc_b;
	wire [4:0] operand_rd_pv_sdotusp_sci_b;
	wire [4:0] operand_rd_pv_sdotsp_h;
	wire [4:0] operand_rd_pv_sdotsp_sc_h;
	wire [4:0] operand_rd_pv_sdotsp_sci_h;
	wire [4:0] operand_rd_pv_sdotsp_b;
	wire [4:0] operand_rd_pv_sdotsp_sc_b;
	wire [4:0] operand_rd_pv_sdotsp_sci_b;
	wire [4:0] operand_rd_pv_shuffle_h;
	wire [4:0] operand_rd_pv_shuffle_sci_h;
	wire [4:0] operand_rd_pv_shuffle_b;
	wire [4:0] operand_rd_pv_shufflei0_sci_b;
	wire [4:0] operand_rd_pv_shufflei1_sci_b;
	wire [4:0] operand_rd_pv_shufflei2_sci_b;
	wire [4:0] operand_rd_pv_shufflei3_sci_b;
	wire [4:0] operand_rd_pv_shuffle2_h;
	wire [4:0] operand_rd_pv_shuffle2_b;
	wire [4:0] operand_rd_pv_pack_h;
	wire [4:0] operand_rd_pv_packhi_b;
	wire [4:0] operand_rd_pv_packlo_b;
	wire [4:0] operand_rs1_jalr;
	wire [4:0] operand_rs1_beq;
	wire [4:0] operand_rs1_bne;
	wire [4:0] operand_rs1_blt;
	wire [4:0] operand_rs1_bge;
	wire [4:0] operand_rs1_bltu;
	wire [4:0] operand_rs1_bgeu;
	wire [4:0] operand_rs1_lb;
	wire [4:0] operand_rs1_lh;
	wire [4:0] operand_rs1_lw;
	wire [4:0] operand_rs1_lbu;
	wire [4:0] operand_rs1_lhu;
	wire [4:0] operand_rs1_sb;
	wire [4:0] operand_rs1_sh;
	wire [4:0] operand_rs1_sw;
	wire [4:0] operand_rs1_addi;
	wire [4:0] operand_rs1_slti;
	wire [4:0] operand_rs1_sltiu;
	wire [4:0] operand_rs1_xori;
	wire [4:0] operand_rs1_ori;
	wire [4:0] operand_rs1_andi;
	wire [4:0] operand_rs1_slli;
	wire [4:0] operand_rs1_srli;
	wire [4:0] operand_rs1_srai;
	wire [4:0] operand_rs1_add;
	wire [4:0] operand_rs1_sub;
	wire [4:0] operand_rs1_sll;
	wire [4:0] operand_rs1_slt;
	wire [4:0] operand_rs1_sltu;
	wire [4:0] operand_rs1_xor;
	wire [4:0] operand_rs1_srl;
	wire [4:0] operand_rs1_sra;
	wire [4:0] operand_rs1_or;
	wire [4:0] operand_rs1_and;
	wire [4:0] operand_rs1_fence;
	wire [4:0] operand_rs1_mul;
	wire [4:0] operand_rs1_mulh;
	wire [4:0] operand_rs1_mulhsu;
	wire [4:0] operand_rs1_mulhu;
	wire [4:0] operand_rs1_div;
	wire [4:0] operand_rs1_divu;
	wire [4:0] operand_rs1_rem;
	wire [4:0] operand_rs1_remu;
	wire [4:0] operand_rs1_csrrw;
	wire [4:0] operand_rs1_csrrs;
	wire [4:0] operand_rs1_csrrc;
	wire [4:0] operand_rs1_p_extract;
	wire [4:0] operand_rs1_p_extractu;
	wire [4:0] operand_rs1_p_insert;
	wire [4:0] operand_rs1_p_bclr;
	wire [4:0] operand_rs1_p_bset;
	wire [4:0] operand_rs1_p_extractr;
	wire [4:0] operand_rs1_p_extractur;
	wire [4:0] operand_rs1_p_insertr;
	wire [4:0] operand_rs1_p_bclrr;
	wire [4:0] operand_rs1_p_bsetr;
	wire [4:0] operand_rs1_p_ror;
	wire [4:0] operand_rs1_p_ff1;
	wire [4:0] operand_rs1_p_fl1;
	wire [4:0] operand_rs1_p_clb;
	wire [4:0] operand_rs1_p_cnt;
	wire [4:0] operand_rs1_p_abs;
	wire [4:0] operand_rs1_p_slet;
	wire [4:0] operand_rs1_p_sletu;
	wire [4:0] operand_rs1_p_min;
	wire [4:0] operand_rs1_p_minu;
	wire [4:0] operand_rs1_p_max;
	wire [4:0] operand_rs1_p_maxu;
	wire [4:0] operand_rs1_p_exths;
	wire [4:0] operand_rs1_p_exthz;
	wire [4:0] operand_rs1_p_extbs;
	wire [4:0] operand_rs1_p_extbz;
	wire [4:0] operand_rs1_p_clip;
	wire [4:0] operand_rs1_p_clipu;
	wire [4:0] operand_rs1_p_clipr;
	wire [4:0] operand_rs1_p_clipur;
	wire [4:0] operand_rs1_p_addn;
	wire [4:0] operand_rs1_p_addun;
	wire [4:0] operand_rs1_p_addrn;
	wire [4:0] operand_rs1_p_addurn;
	wire [4:0] operand_rs1_p_subn;
	wire [4:0] operand_rs1_p_subun;
	wire [4:0] operand_rs1_p_subrn;
	wire [4:0] operand_rs1_p_suburn;
	wire [4:0] operand_rs1_p_addnr;
	wire [4:0] operand_rs1_p_addunr;
	wire [4:0] operand_rs1_p_addrnr;
	wire [4:0] operand_rs1_p_addurnr;
	wire [4:0] operand_rs1_p_subnr;
	wire [4:0] operand_rs1_p_subunr;
	wire [4:0] operand_rs1_p_subrnr;
	wire [4:0] operand_rs1_p_suburnr;
	wire [4:0] operand_rs1_lp_count;
	wire [4:0] operand_rs1_lp_setup;
	wire [4:0] operand_rs1_p_beqimm;
	wire [4:0] operand_rs1_p_bneimm;
	wire [4:0] operand_rs1_p_lb_i_inc;
	wire [4:0] operand_rs1_p_lbu_i_inc;
	wire [4:0] operand_rs1_p_lh_i_inc;
	wire [4:0] operand_rs1_p_lhu_i_inc;
	wire [4:0] operand_rs1_p_lw_i_inc;
	wire [4:0] operand_rs1_p_lb_r_inc;
	wire [4:0] operand_rs1_p_lbu_r_inc;
	wire [4:0] operand_rs1_p_lh_r_inc;
	wire [4:0] operand_rs1_p_lhu_r_inc;
	wire [4:0] operand_rs1_p_lw_r_inc;
	wire [4:0] operand_rs1_p_lb_r;
	wire [4:0] operand_rs1_p_lbu_r;
	wire [4:0] operand_rs1_p_lh_r;
	wire [4:0] operand_rs1_p_lhu_r;
	wire [4:0] operand_rs1_p_lw_r;
	wire [4:0] operand_rs1_p_mulu;
	wire [4:0] operand_rs1_p_mulhhu;
	wire [4:0] operand_rs1_p_mulun;
	wire [4:0] operand_rs1_p_mulhhun;
	wire [4:0] operand_rs1_p_mulurn;
	wire [4:0] operand_rs1_p_mulhhurn;
	wire [4:0] operand_rs1_p_macsn;
	wire [4:0] operand_rs1_p_machhsn;
	wire [4:0] operand_rs1_p_macsrn;
	wire [4:0] operand_rs1_p_machhsrn;
	wire [4:0] operand_rs1_p_macun;
	wire [4:0] operand_rs1_p_machhun;
	wire [4:0] operand_rs1_p_macurn;
	wire [4:0] operand_rs1_p_machhurn;
	wire [4:0] operand_rs1_p_sb_i_inc;
	wire [4:0] operand_rs1_p_sh_i_inc;
	wire [4:0] operand_rs1_p_sw_i_inc;
	wire [4:0] operand_rs1_p_sb_r_inc;
	wire [4:0] operand_rs1_p_sh_r_inc;
	wire [4:0] operand_rs1_p_sw_r_inc;
	wire [4:0] operand_rs1_p_sb_r;
	wire [4:0] operand_rs1_p_sh_r;
	wire [4:0] operand_rs1_p_sw_r;
	wire [4:0] operand_rs1_pv_cmpeq_h;
	wire [4:0] operand_rs1_pv_cmpeq_sc_h;
	wire [4:0] operand_rs1_pv_cmpeq_sci_h;
	wire [4:0] operand_rs1_pv_cmpeq_b;
	wire [4:0] operand_rs1_pv_cmpeq_sc_b;
	wire [4:0] operand_rs1_pv_cmpeq_sci_b;
	wire [4:0] operand_rs1_pv_cmpne_h;
	wire [4:0] operand_rs1_pv_cmpne_sc_h;
	wire [4:0] operand_rs1_pv_cmpne_sci_h;
	wire [4:0] operand_rs1_pv_cmpne_b;
	wire [4:0] operand_rs1_pv_cmpne_sc_b;
	wire [4:0] operand_rs1_pv_cmpne_sci_b;
	wire [4:0] operand_rs1_pv_cmpgt_h;
	wire [4:0] operand_rs1_pv_cmpgt_sc_h;
	wire [4:0] operand_rs1_pv_cmpgt_sci_h;
	wire [4:0] operand_rs1_pv_cmpgt_b;
	wire [4:0] operand_rs1_pv_cmpgt_sc_b;
	wire [4:0] operand_rs1_pv_cmpgt_sci_b;
	wire [4:0] operand_rs1_pv_cmpge_h;
	wire [4:0] operand_rs1_pv_cmpge_sc_h;
	wire [4:0] operand_rs1_pv_cmpge_sci_h;
	wire [4:0] operand_rs1_pv_cmpge_b;
	wire [4:0] operand_rs1_pv_cmpge_sc_b;
	wire [4:0] operand_rs1_pv_cmpge_sci_b;
	wire [4:0] operand_rs1_pv_cmplt_h;
	wire [4:0] operand_rs1_pv_cmplt_sc_h;
	wire [4:0] operand_rs1_pv_cmplt_sci_h;
	wire [4:0] operand_rs1_pv_cmplt_b;
	wire [4:0] operand_rs1_pv_cmplt_sc_b;
	wire [4:0] operand_rs1_pv_cmplt_sci_b;
	wire [4:0] operand_rs1_pv_cmple_h;
	wire [4:0] operand_rs1_pv_cmple_sc_h;
	wire [4:0] operand_rs1_pv_cmple_sci_h;
	wire [4:0] operand_rs1_pv_cmple_b;
	wire [4:0] operand_rs1_pv_cmple_sc_b;
	wire [4:0] operand_rs1_pv_cmple_sci_b;
	wire [4:0] operand_rs1_pv_cmpgtu_h;
	wire [4:0] operand_rs1_pv_cmpgtu_sc_h;
	wire [4:0] operand_rs1_pv_cmpgtu_sci_h;
	wire [4:0] operand_rs1_pv_cmpgtu_b;
	wire [4:0] operand_rs1_pv_cmpgtu_sc_b;
	wire [4:0] operand_rs1_pv_cmpgtu_sci_b;
	wire [4:0] operand_rs1_pv_cmpgeu_h;
	wire [4:0] operand_rs1_pv_cmpgeu_sc_h;
	wire [4:0] operand_rs1_pv_cmpgeu_sci_h;
	wire [4:0] operand_rs1_pv_cmpgeu_b;
	wire [4:0] operand_rs1_pv_cmpgeu_sc_b;
	wire [4:0] operand_rs1_pv_cmpgeu_sci_b;
	wire [4:0] operand_rs1_pv_cmpltu_h;
	wire [4:0] operand_rs1_pv_cmpltu_sc_h;
	wire [4:0] operand_rs1_pv_cmpltu_sci_h;
	wire [4:0] operand_rs1_pv_cmpltu_b;
	wire [4:0] operand_rs1_pv_cmpltu_sc_b;
	wire [4:0] operand_rs1_pv_cmpltu_sci_b;
	wire [4:0] operand_rs1_pv_cmpleu_h;
	wire [4:0] operand_rs1_pv_cmpleu_sc_h;
	wire [4:0] operand_rs1_pv_cmpleu_sci_h;
	wire [4:0] operand_rs1_pv_cmpleu_b;
	wire [4:0] operand_rs1_pv_cmpleu_sc_b;
	wire [4:0] operand_rs1_pv_cmpleu_sci_b;
	wire [4:0] operand_rs1_pv_add_h;
	wire [4:0] operand_rs1_pv_add_sc_h;
	wire [4:0] operand_rs1_pv_add_sci_h;
	wire [4:0] operand_rs1_pv_add_b;
	wire [4:0] operand_rs1_pv_add_sc_b;
	wire [4:0] operand_rs1_pv_add_sci_b;
	wire [4:0] operand_rs1_pv_sub_h;
	wire [4:0] operand_rs1_pv_sub_sc_h;
	wire [4:0] operand_rs1_pv_sub_sci_h;
	wire [4:0] operand_rs1_pv_sub_b;
	wire [4:0] operand_rs1_pv_sub_sc_b;
	wire [4:0] operand_rs1_pv_sub_sci_b;
	wire [4:0] operand_rs1_pv_avg_h;
	wire [4:0] operand_rs1_pv_avg_sc_h;
	wire [4:0] operand_rs1_pv_avg_sci_h;
	wire [4:0] operand_rs1_pv_avg_b;
	wire [4:0] operand_rs1_pv_avg_sc_b;
	wire [4:0] operand_rs1_pv_avg_sci_b;
	wire [4:0] operand_rs1_pv_avgu_h;
	wire [4:0] operand_rs1_pv_avgu_sc_h;
	wire [4:0] operand_rs1_pv_avgu_sci_h;
	wire [4:0] operand_rs1_pv_avgu_b;
	wire [4:0] operand_rs1_pv_avgu_sc_b;
	wire [4:0] operand_rs1_pv_avgu_sci_b;
	wire [4:0] operand_rs1_pv_min_h;
	wire [4:0] operand_rs1_pv_min_sc_h;
	wire [4:0] operand_rs1_pv_min_sci_h;
	wire [4:0] operand_rs1_pv_min_b;
	wire [4:0] operand_rs1_pv_min_sc_b;
	wire [4:0] operand_rs1_pv_min_sci_b;
	wire [4:0] operand_rs1_pv_minu_h;
	wire [4:0] operand_rs1_pv_minu_sc_h;
	wire [4:0] operand_rs1_pv_minu_sci_h;
	wire [4:0] operand_rs1_pv_minu_b;
	wire [4:0] operand_rs1_pv_minu_sc_b;
	wire [4:0] operand_rs1_pv_minu_sci_b;
	wire [4:0] operand_rs1_pv_max_h;
	wire [4:0] operand_rs1_pv_max_sc_h;
	wire [4:0] operand_rs1_pv_max_sci_h;
	wire [4:0] operand_rs1_pv_max_b;
	wire [4:0] operand_rs1_pv_max_sc_b;
	wire [4:0] operand_rs1_pv_max_sci_b;
	wire [4:0] operand_rs1_pv_maxu_h;
	wire [4:0] operand_rs1_pv_maxu_sc_h;
	wire [4:0] operand_rs1_pv_maxu_sci_h;
	wire [4:0] operand_rs1_pv_maxu_b;
	wire [4:0] operand_rs1_pv_maxu_sc_b;
	wire [4:0] operand_rs1_pv_maxu_sci_b;
	wire [4:0] operand_rs1_pv_srl_h;
	wire [4:0] operand_rs1_pv_srl_sc_h;
	wire [4:0] operand_rs1_pv_srl_sci_h;
	wire [4:0] operand_rs1_pv_srl_b;
	wire [4:0] operand_rs1_pv_srl_sc_b;
	wire [4:0] operand_rs1_pv_srl_sci_b;
	wire [4:0] operand_rs1_pv_sra_h;
	wire [4:0] operand_rs1_pv_sra_sc_h;
	wire [4:0] operand_rs1_pv_sra_sci_h;
	wire [4:0] operand_rs1_pv_sra_b;
	wire [4:0] operand_rs1_pv_sra_sc_b;
	wire [4:0] operand_rs1_pv_sra_sci_b;
	wire [4:0] operand_rs1_pv_sll_h;
	wire [4:0] operand_rs1_pv_sll_sc_h;
	wire [4:0] operand_rs1_pv_sll_sci_h;
	wire [4:0] operand_rs1_pv_sll_b;
	wire [4:0] operand_rs1_pv_sll_sc_b;
	wire [4:0] operand_rs1_pv_sll_sci_b;
	wire [4:0] operand_rs1_pv_or_h;
	wire [4:0] operand_rs1_pv_or_sc_h;
	wire [4:0] operand_rs1_pv_or_sci_h;
	wire [4:0] operand_rs1_pv_or_b;
	wire [4:0] operand_rs1_pv_or_sc_b;
	wire [4:0] operand_rs1_pv_or_sci_b;
	wire [4:0] operand_rs1_pv_xor_h;
	wire [4:0] operand_rs1_pv_xor_sc_h;
	wire [4:0] operand_rs1_pv_xor_sci_h;
	wire [4:0] operand_rs1_pv_xor_b;
	wire [4:0] operand_rs1_pv_xor_sc_b;
	wire [4:0] operand_rs1_pv_xor_sci_b;
	wire [4:0] operand_rs1_pv_and_h;
	wire [4:0] operand_rs1_pv_and_sc_h;
	wire [4:0] operand_rs1_pv_and_sci_h;
	wire [4:0] operand_rs1_pv_and_b;
	wire [4:0] operand_rs1_pv_and_sc_b;
	wire [4:0] operand_rs1_pv_and_sci_b;
	wire [4:0] operand_rs1_pv_abs_h;
	wire [4:0] operand_rs1_pv_abs_b;
	wire [4:0] operand_rs1_pv_extract_h;
	wire [4:0] operand_rs1_pv_extract_b;
	wire [4:0] operand_rs1_pv_extractu_h;
	wire [4:0] operand_rs1_pv_extractu_b;
	wire [4:0] operand_rs1_pv_insert_h;
	wire [4:0] operand_rs1_pv_insert_b;
	wire [4:0] operand_rs1_pv_dotup_h;
	wire [4:0] operand_rs1_pv_dotup_sc_h;
	wire [4:0] operand_rs1_pv_dotup_sci_h;
	wire [4:0] operand_rs1_pv_dotup_b;
	wire [4:0] operand_rs1_pv_dotup_sc_b;
	wire [4:0] operand_rs1_pv_dotup_sci_b;
	wire [4:0] operand_rs1_pv_dotusp_h;
	wire [4:0] operand_rs1_pv_dotusp_sc_h;
	wire [4:0] operand_rs1_pv_dotusp_sci_h;
	wire [4:0] operand_rs1_pv_dotusp_b;
	wire [4:0] operand_rs1_pv_dotusp_sc_b;
	wire [4:0] operand_rs1_pv_dotusp_sci_b;
	wire [4:0] operand_rs1_pv_dotsp_h;
	wire [4:0] operand_rs1_pv_dotsp_sc_h;
	wire [4:0] operand_rs1_pv_dotsp_sci_h;
	wire [4:0] operand_rs1_pv_dotsp_b;
	wire [4:0] operand_rs1_pv_dotsp_sc_b;
	wire [4:0] operand_rs1_pv_dotsp_sci_b;
	wire [4:0] operand_rs1_pv_sdotup_h;
	wire [4:0] operand_rs1_pv_sdotup_sc_h;
	wire [4:0] operand_rs1_pv_sdotup_sci_h;
	wire [4:0] operand_rs1_pv_sdotup_b;
	wire [4:0] operand_rs1_pv_sdotup_sc_b;
	wire [4:0] operand_rs1_pv_sdotup_sci_b;
	wire [4:0] operand_rs1_pv_sdotusp_h;
	wire [4:0] operand_rs1_pv_sdotusp_sc_h;
	wire [4:0] operand_rs1_pv_sdotusp_sci_h;
	wire [4:0] operand_rs1_pv_sdotusp_b;
	wire [4:0] operand_rs1_pv_sdotusp_sc_b;
	wire [4:0] operand_rs1_pv_sdotusp_sci_b;
	wire [4:0] operand_rs1_pv_sdotsp_h;
	wire [4:0] operand_rs1_pv_sdotsp_sc_h;
	wire [4:0] operand_rs1_pv_sdotsp_sci_h;
	wire [4:0] operand_rs1_pv_sdotsp_b;
	wire [4:0] operand_rs1_pv_sdotsp_sc_b;
	wire [4:0] operand_rs1_pv_sdotsp_sci_b;
	wire [4:0] operand_rs1_pv_shuffle_h;
	wire [4:0] operand_rs1_pv_shuffle_sci_h;
	wire [4:0] operand_rs1_pv_shuffle_b;
	wire [4:0] operand_rs1_pv_shufflei0_sci_b;
	wire [4:0] operand_rs1_pv_shufflei1_sci_b;
	wire [4:0] operand_rs1_pv_shufflei2_sci_b;
	wire [4:0] operand_rs1_pv_shufflei3_sci_b;
	wire [4:0] operand_rs1_pv_shuffle2_h;
	wire [4:0] operand_rs1_pv_shuffle2_b;
	wire [4:0] operand_rs1_pv_pack_h;
	wire [4:0] operand_rs1_pv_packhi_b;
	wire [4:0] operand_rs1_pv_packlo_b;
	wire [4:0] operand_rs2_beq;
	wire [4:0] operand_rs2_bne;
	wire [4:0] operand_rs2_blt;
	wire [4:0] operand_rs2_bge;
	wire [4:0] operand_rs2_bltu;
	wire [4:0] operand_rs2_bgeu;
	wire [4:0] operand_rs2_sb;
	wire [4:0] operand_rs2_sh;
	wire [4:0] operand_rs2_sw;
	wire [4:0] operand_rs2_add;
	wire [4:0] operand_rs2_sub;
	wire [4:0] operand_rs2_sll;
	wire [4:0] operand_rs2_slt;
	wire [4:0] operand_rs2_sltu;
	wire [4:0] operand_rs2_xor;
	wire [4:0] operand_rs2_srl;
	wire [4:0] operand_rs2_sra;
	wire [4:0] operand_rs2_or;
	wire [4:0] operand_rs2_and;
	wire [4:0] operand_rs2_mul;
	wire [4:0] operand_rs2_mulh;
	wire [4:0] operand_rs2_mulhsu;
	wire [4:0] operand_rs2_mulhu;
	wire [4:0] operand_rs2_div;
	wire [4:0] operand_rs2_divu;
	wire [4:0] operand_rs2_rem;
	wire [4:0] operand_rs2_remu;
	wire [4:0] operand_rs2_p_extractr;
	wire [4:0] operand_rs2_p_extractur;
	wire [4:0] operand_rs2_p_insertr;
	wire [4:0] operand_rs2_p_bclrr;
	wire [4:0] operand_rs2_p_bsetr;
	wire [4:0] operand_rs2_p_ror;
	wire [4:0] operand_rs2_p_slet;
	wire [4:0] operand_rs2_p_sletu;
	wire [4:0] operand_rs2_p_min;
	wire [4:0] operand_rs2_p_minu;
	wire [4:0] operand_rs2_p_max;
	wire [4:0] operand_rs2_p_maxu;
	wire [4:0] operand_rs2_p_clipr;
	wire [4:0] operand_rs2_p_clipur;
	wire [4:0] operand_rs2_p_addn;
	wire [4:0] operand_rs2_p_addun;
	wire [4:0] operand_rs2_p_addrn;
	wire [4:0] operand_rs2_p_addurn;
	wire [4:0] operand_rs2_p_subn;
	wire [4:0] operand_rs2_p_subun;
	wire [4:0] operand_rs2_p_subrn;
	wire [4:0] operand_rs2_p_suburn;
	wire [4:0] operand_rs2_p_addnr;
	wire [4:0] operand_rs2_p_addunr;
	wire [4:0] operand_rs2_p_addrnr;
	wire [4:0] operand_rs2_p_addurnr;
	wire [4:0] operand_rs2_p_subnr;
	wire [4:0] operand_rs2_p_subunr;
	wire [4:0] operand_rs2_p_subrnr;
	wire [4:0] operand_rs2_p_suburnr;
	wire [4:0] operand_rs2_p_lb_r_inc;
	wire [4:0] operand_rs2_p_lbu_r_inc;
	wire [4:0] operand_rs2_p_lh_r_inc;
	wire [4:0] operand_rs2_p_lhu_r_inc;
	wire [4:0] operand_rs2_p_lw_r_inc;
	wire [4:0] operand_rs2_p_lb_r;
	wire [4:0] operand_rs2_p_lbu_r;
	wire [4:0] operand_rs2_p_lh_r;
	wire [4:0] operand_rs2_p_lhu_r;
	wire [4:0] operand_rs2_p_lw_r;
	wire [4:0] operand_rs2_p_mulu;
	wire [4:0] operand_rs2_p_mulhhu;
	wire [4:0] operand_rs2_p_mulun;
	wire [4:0] operand_rs2_p_mulhhun;
	wire [4:0] operand_rs2_p_mulurn;
	wire [4:0] operand_rs2_p_mulhhurn;
	wire [4:0] operand_rs2_p_macsn;
	wire [4:0] operand_rs2_p_machhsn;
	wire [4:0] operand_rs2_p_macsrn;
	wire [4:0] operand_rs2_p_machhsrn;
	wire [4:0] operand_rs2_p_macun;
	wire [4:0] operand_rs2_p_machhun;
	wire [4:0] operand_rs2_p_macurn;
	wire [4:0] operand_rs2_p_machhurn;
	wire [4:0] operand_rs2_p_sb_i_inc;
	wire [4:0] operand_rs2_p_sh_i_inc;
	wire [4:0] operand_rs2_p_sw_i_inc;
	wire [4:0] operand_rs2_p_sb_r_inc;
	wire [4:0] operand_rs2_p_sh_r_inc;
	wire [4:0] operand_rs2_p_sw_r_inc;
	wire [4:0] operand_rs2_p_sb_r;
	wire [4:0] operand_rs2_p_sh_r;
	wire [4:0] operand_rs2_p_sw_r;
	wire [4:0] operand_rs2_pv_cmpeq_h;
	wire [4:0] operand_rs2_pv_cmpeq_sc_h;
	wire [4:0] operand_rs2_pv_cmpeq_b;
	wire [4:0] operand_rs2_pv_cmpeq_sc_b;
	wire [4:0] operand_rs2_pv_cmpne_h;
	wire [4:0] operand_rs2_pv_cmpne_sc_h;
	wire [4:0] operand_rs2_pv_cmpne_b;
	wire [4:0] operand_rs2_pv_cmpne_sc_b;
	wire [4:0] operand_rs2_pv_cmpgt_h;
	wire [4:0] operand_rs2_pv_cmpgt_sc_h;
	wire [4:0] operand_rs2_pv_cmpgt_b;
	wire [4:0] operand_rs2_pv_cmpgt_sc_b;
	wire [4:0] operand_rs2_pv_cmpge_h;
	wire [4:0] operand_rs2_pv_cmpge_sc_h;
	wire [4:0] operand_rs2_pv_cmpge_b;
	wire [4:0] operand_rs2_pv_cmpge_sc_b;
	wire [4:0] operand_rs2_pv_cmplt_h;
	wire [4:0] operand_rs2_pv_cmplt_sc_h;
	wire [4:0] operand_rs2_pv_cmplt_b;
	wire [4:0] operand_rs2_pv_cmplt_sc_b;
	wire [4:0] operand_rs2_pv_cmple_h;
	wire [4:0] operand_rs2_pv_cmple_sc_h;
	wire [4:0] operand_rs2_pv_cmple_b;
	wire [4:0] operand_rs2_pv_cmple_sc_b;
	wire [4:0] operand_rs2_pv_cmpgtu_h;
	wire [4:0] operand_rs2_pv_cmpgtu_sc_h;
	wire [4:0] operand_rs2_pv_cmpgtu_b;
	wire [4:0] operand_rs2_pv_cmpgtu_sc_b;
	wire [4:0] operand_rs2_pv_cmpgeu_h;
	wire [4:0] operand_rs2_pv_cmpgeu_sc_h;
	wire [4:0] operand_rs2_pv_cmpgeu_b;
	wire [4:0] operand_rs2_pv_cmpgeu_sc_b;
	wire [4:0] operand_rs2_pv_cmpltu_h;
	wire [4:0] operand_rs2_pv_cmpltu_sc_h;
	wire [4:0] operand_rs2_pv_cmpltu_b;
	wire [4:0] operand_rs2_pv_cmpltu_sc_b;
	wire [4:0] operand_rs2_pv_cmpleu_h;
	wire [4:0] operand_rs2_pv_cmpleu_sc_h;
	wire [4:0] operand_rs2_pv_cmpleu_b;
	wire [4:0] operand_rs2_pv_cmpleu_sc_b;
	wire [4:0] operand_rs2_pv_add_h;
	wire [4:0] operand_rs2_pv_add_sc_h;
	wire [4:0] operand_rs2_pv_add_b;
	wire [4:0] operand_rs2_pv_add_sc_b;
	wire [4:0] operand_rs2_pv_sub_h;
	wire [4:0] operand_rs2_pv_sub_sc_h;
	wire [4:0] operand_rs2_pv_sub_b;
	wire [4:0] operand_rs2_pv_sub_sc_b;
	wire [4:0] operand_rs2_pv_avg_h;
	wire [4:0] operand_rs2_pv_avg_sc_h;
	wire [4:0] operand_rs2_pv_avg_b;
	wire [4:0] operand_rs2_pv_avg_sc_b;
	wire [4:0] operand_rs2_pv_avgu_h;
	wire [4:0] operand_rs2_pv_avgu_sc_h;
	wire [4:0] operand_rs2_pv_avgu_b;
	wire [4:0] operand_rs2_pv_avgu_sc_b;
	wire [4:0] operand_rs2_pv_min_h;
	wire [4:0] operand_rs2_pv_min_sc_h;
	wire [4:0] operand_rs2_pv_min_b;
	wire [4:0] operand_rs2_pv_min_sc_b;
	wire [4:0] operand_rs2_pv_minu_h;
	wire [4:0] operand_rs2_pv_minu_sc_h;
	wire [4:0] operand_rs2_pv_minu_b;
	wire [4:0] operand_rs2_pv_minu_sc_b;
	wire [4:0] operand_rs2_pv_max_h;
	wire [4:0] operand_rs2_pv_max_sc_h;
	wire [4:0] operand_rs2_pv_max_b;
	wire [4:0] operand_rs2_pv_max_sc_b;
	wire [4:0] operand_rs2_pv_maxu_h;
	wire [4:0] operand_rs2_pv_maxu_sc_h;
	wire [4:0] operand_rs2_pv_maxu_b;
	wire [4:0] operand_rs2_pv_maxu_sc_b;
	wire [4:0] operand_rs2_pv_srl_h;
	wire [4:0] operand_rs2_pv_srl_sc_h;
	wire [4:0] operand_rs2_pv_srl_b;
	wire [4:0] operand_rs2_pv_srl_sc_b;
	wire [4:0] operand_rs2_pv_sra_h;
	wire [4:0] operand_rs2_pv_sra_sc_h;
	wire [4:0] operand_rs2_pv_sra_b;
	wire [4:0] operand_rs2_pv_sra_sc_b;
	wire [4:0] operand_rs2_pv_sll_h;
	wire [4:0] operand_rs2_pv_sll_sc_h;
	wire [4:0] operand_rs2_pv_sll_b;
	wire [4:0] operand_rs2_pv_sll_sc_b;
	wire [4:0] operand_rs2_pv_or_h;
	wire [4:0] operand_rs2_pv_or_sc_h;
	wire [4:0] operand_rs2_pv_or_b;
	wire [4:0] operand_rs2_pv_or_sc_b;
	wire [4:0] operand_rs2_pv_xor_h;
	wire [4:0] operand_rs2_pv_xor_sc_h;
	wire [4:0] operand_rs2_pv_xor_b;
	wire [4:0] operand_rs2_pv_xor_sc_b;
	wire [4:0] operand_rs2_pv_and_h;
	wire [4:0] operand_rs2_pv_and_sc_h;
	wire [4:0] operand_rs2_pv_and_b;
	wire [4:0] operand_rs2_pv_and_sc_b;
	wire [4:0] operand_rs2_pv_dotup_h;
	wire [4:0] operand_rs2_pv_dotup_sc_h;
	wire [4:0] operand_rs2_pv_dotup_b;
	wire [4:0] operand_rs2_pv_dotup_sc_b;
	wire [4:0] operand_rs2_pv_dotusp_h;
	wire [4:0] operand_rs2_pv_dotusp_sc_h;
	wire [4:0] operand_rs2_pv_dotusp_b;
	wire [4:0] operand_rs2_pv_dotusp_sc_b;
	wire [4:0] operand_rs2_pv_dotsp_h;
	wire [4:0] operand_rs2_pv_dotsp_sc_h;
	wire [4:0] operand_rs2_pv_dotsp_b;
	wire [4:0] operand_rs2_pv_dotsp_sc_b;
	wire [4:0] operand_rs2_pv_sdotup_h;
	wire [4:0] operand_rs2_pv_sdotup_sc_h;
	wire [4:0] operand_rs2_pv_sdotup_b;
	wire [4:0] operand_rs2_pv_sdotup_sc_b;
	wire [4:0] operand_rs2_pv_sdotusp_h;
	wire [4:0] operand_rs2_pv_sdotusp_sc_h;
	wire [4:0] operand_rs2_pv_sdotusp_b;
	wire [4:0] operand_rs2_pv_sdotusp_sc_b;
	wire [4:0] operand_rs2_pv_sdotsp_h;
	wire [4:0] operand_rs2_pv_sdotsp_sc_h;
	wire [4:0] operand_rs2_pv_sdotsp_b;
	wire [4:0] operand_rs2_pv_sdotsp_sc_b;
	wire [4:0] operand_rs2_pv_shuffle_h;
	wire [4:0] operand_rs2_pv_shuffle_b;
	wire [4:0] operand_rs2_pv_shuffle2_h;
	wire [4:0] operand_rs2_pv_shuffle2_b;
	wire [4:0] operand_rs2_pv_pack_h;
	wire [4:0] operand_rs2_pv_packhi_b;
	wire [4:0] operand_rs2_pv_packlo_b;
	wire [11:0] operand_csr_csrrw;
	wire [11:0] operand_csr_csrrs;
	wire [11:0] operand_csr_csrrc;
	wire [11:0] operand_csr_csrrwi;
	wire [11:0] operand_csr_csrrsi;
	wire [11:0] operand_csr_csrrci;


	// Instance for lui instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lui lui_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lui_instruction)
		, .operand_imm(operand_imm_lui)
		, .operand_rd(operand_rd_lui)
	);

	// Instance for auipc instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_auipc auipc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_auipc_instruction)
		, .operand_imm(operand_imm_auipc)
		, .operand_rd(operand_rd_auipc)
	);

	// Instance for jal instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_jal jal_inst (
		  .opcode(opcode)
		, .opcode_valid(is_jal_instruction)
		, .operand_imm(operand_imm_jal)
		, .operand_rd(operand_rd_jal)
	);

	// Instance for jalr instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_jalr jalr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_jalr_instruction)
		, .operand_imm(operand_imm_jalr)
		, .operand_rs1(operand_rs1_jalr)
		, .operand_rd(operand_rd_jalr)
	);

	// Instance for beq instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_beq beq_inst (
		  .opcode(opcode)
		, .opcode_valid(is_beq_instruction)
		, .operand_imm(operand_imm_beq)
		, .operand_rs2(operand_rs2_beq)
		, .operand_rs1(operand_rs1_beq)
	);

	// Instance for bne instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_bne bne_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bne_instruction)
		, .operand_imm(operand_imm_bne)
		, .operand_rs2(operand_rs2_bne)
		, .operand_rs1(operand_rs1_bne)
	);

	// Instance for blt instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_blt blt_inst (
		  .opcode(opcode)
		, .opcode_valid(is_blt_instruction)
		, .operand_imm(operand_imm_blt)
		, .operand_rs2(operand_rs2_blt)
		, .operand_rs1(operand_rs1_blt)
	);

	// Instance for bge instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_bge bge_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bge_instruction)
		, .operand_imm(operand_imm_bge)
		, .operand_rs2(operand_rs2_bge)
		, .operand_rs1(operand_rs1_bge)
	);

	// Instance for bltu instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_bltu bltu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bltu_instruction)
		, .operand_imm(operand_imm_bltu)
		, .operand_rs2(operand_rs2_bltu)
		, .operand_rs1(operand_rs1_bltu)
	);

	// Instance for bgeu instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_bgeu bgeu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bgeu_instruction)
		, .operand_imm(operand_imm_bgeu)
		, .operand_rs2(operand_rs2_bgeu)
		, .operand_rs1(operand_rs1_bgeu)
	);

	// Instance for lb instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lb lb_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lb_instruction)
		, .operand_imm(operand_imm_lb)
		, .operand_rs1(operand_rs1_lb)
		, .operand_rd(operand_rd_lb)
	);

	// Instance for lh instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lh lh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lh_instruction)
		, .operand_imm(operand_imm_lh)
		, .operand_rs1(operand_rs1_lh)
		, .operand_rd(operand_rd_lh)
	);

	// Instance for lw instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lw lw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lw_instruction)
		, .operand_imm(operand_imm_lw)
		, .operand_rs1(operand_rs1_lw)
		, .operand_rd(operand_rd_lw)
	);

	// Instance for lbu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lbu lbu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lbu_instruction)
		, .operand_imm(operand_imm_lbu)
		, .operand_rs1(operand_rs1_lbu)
		, .operand_rd(operand_rd_lbu)
	);

	// Instance for lhu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lhu lhu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lhu_instruction)
		, .operand_imm(operand_imm_lhu)
		, .operand_rs1(operand_rs1_lhu)
		, .operand_rd(operand_rd_lhu)
	);

	// Instance for sb instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sb sb_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sb_instruction)
		, .operand_imm(operand_imm_sb)
		, .operand_rs2(operand_rs2_sb)
		, .operand_rs1(operand_rs1_sb)
	);

	// Instance for sh instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sh sh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sh_instruction)
		, .operand_imm(operand_imm_sh)
		, .operand_rs2(operand_rs2_sh)
		, .operand_rs1(operand_rs1_sh)
	);

	// Instance for sw instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sw sw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sw_instruction)
		, .operand_imm(operand_imm_sw)
		, .operand_rs2(operand_rs2_sw)
		, .operand_rs1(operand_rs1_sw)
	);

	// Instance for addi instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_addi addi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_addi_instruction)
		, .operand_imm(operand_imm_addi)
		, .operand_rs1(operand_rs1_addi)
		, .operand_rd(operand_rd_addi)
	);

	// Instance for slti instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_slti slti_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slti_instruction)
		, .operand_imm(operand_imm_slti)
		, .operand_rs1(operand_rs1_slti)
		, .operand_rd(operand_rd_slti)
	);

	// Instance for sltiu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sltiu sltiu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sltiu_instruction)
		, .operand_imm(operand_imm_sltiu)
		, .operand_rs1(operand_rs1_sltiu)
		, .operand_rd(operand_rd_sltiu)
	);

	// Instance for xori instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_xori xori_inst (
		  .opcode(opcode)
		, .opcode_valid(is_xori_instruction)
		, .operand_imm(operand_imm_xori)
		, .operand_rs1(operand_rs1_xori)
		, .operand_rd(operand_rd_xori)
	);

	// Instance for ori instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_ori ori_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ori_instruction)
		, .operand_imm(operand_imm_ori)
		, .operand_rs1(operand_rs1_ori)
		, .operand_rd(operand_rd_ori)
	);

	// Instance for andi instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_andi andi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_andi_instruction)
		, .operand_imm(operand_imm_andi)
		, .operand_rs1(operand_rs1_andi)
		, .operand_rd(operand_rd_andi)
	);

	// Instance for slli instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_slli slli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slli_instruction)
		, .operand_shamt(operand_shamt_slli)
		, .operand_rs1(operand_rs1_slli)
		, .operand_rd(operand_rd_slli)
	);

	// Instance for srli instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_srli srli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srli_instruction)
		, .operand_shamt(operand_shamt_srli)
		, .operand_rs1(operand_rs1_srli)
		, .operand_rd(operand_rd_srli)
	);

	// Instance for srai instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_srai srai_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srai_instruction)
		, .operand_shamt(operand_shamt_srai)
		, .operand_rs1(operand_rs1_srai)
		, .operand_rd(operand_rd_srai)
	);

	// Instance for add instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_add add_inst (
		  .opcode(opcode)
		, .opcode_valid(is_add_instruction)
		, .operand_rs2(operand_rs2_add)
		, .operand_rs1(operand_rs1_add)
		, .operand_rd(operand_rd_add)
	);

	// Instance for sub instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sub sub_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sub_instruction)
		, .operand_rs2(operand_rs2_sub)
		, .operand_rs1(operand_rs1_sub)
		, .operand_rd(operand_rd_sub)
	);

	// Instance for sll instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sll sll_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sll_instruction)
		, .operand_rs2(operand_rs2_sll)
		, .operand_rs1(operand_rs1_sll)
		, .operand_rd(operand_rd_sll)
	);

	// Instance for slt instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_slt slt_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slt_instruction)
		, .operand_rs2(operand_rs2_slt)
		, .operand_rs1(operand_rs1_slt)
		, .operand_rd(operand_rd_slt)
	);

	// Instance for sltu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sltu sltu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sltu_instruction)
		, .operand_rs2(operand_rs2_sltu)
		, .operand_rs1(operand_rs1_sltu)
		, .operand_rd(operand_rd_sltu)
	);

	// Instance for xor instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_xor xor_inst (
		  .opcode(opcode)
		, .opcode_valid(is_xor_instruction)
		, .operand_rs2(operand_rs2_xor)
		, .operand_rs1(operand_rs1_xor)
		, .operand_rd(operand_rd_xor)
	);

	// Instance for srl instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_srl srl_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srl_instruction)
		, .operand_rs2(operand_rs2_srl)
		, .operand_rs1(operand_rs1_srl)
		, .operand_rd(operand_rd_srl)
	);

	// Instance for sra instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_sra sra_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sra_instruction)
		, .operand_rs2(operand_rs2_sra)
		, .operand_rs1(operand_rs1_sra)
		, .operand_rd(operand_rd_sra)
	);

	// Instance for or instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_or or_inst (
		  .opcode(opcode)
		, .opcode_valid(is_or_instruction)
		, .operand_rs2(operand_rs2_or)
		, .operand_rs1(operand_rs1_or)
		, .operand_rd(operand_rd_or)
	);

	// Instance for and instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_and and_inst (
		  .opcode(opcode)
		, .opcode_valid(is_and_instruction)
		, .operand_rs2(operand_rs2_and)
		, .operand_rs1(operand_rs1_and)
		, .operand_rd(operand_rd_and)
	);

	// Instance for fence instruction
	// Operands [fm (4 bits), pred (4 bits), succ (4 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_fence fence_inst (
		  .opcode(opcode)
		, .opcode_valid(is_fence_instruction)
		, .operand_fm(operand_fm_fence)
		, .operand_pred(operand_pred_fence)
		, .operand_succ(operand_succ_fence)
		, .operand_rs1(operand_rs1_fence)
		, .operand_rd(operand_rd_fence)
	);

	// Instance for ecall instruction
	// Operands []
	rv32im_zicsr_xpulp2_ri5cy_ecall ecall_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ecall_instruction)
	);

	// Instance for ebreak instruction
	// Operands []
	rv32im_zicsr_xpulp2_ri5cy_ebreak ebreak_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ebreak_instruction)
	);

	// Instance for mul instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_mul mul_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mul_instruction)
		, .operand_rs2(operand_rs2_mul)
		, .operand_rs1(operand_rs1_mul)
		, .operand_rd(operand_rd_mul)
	);

	// Instance for mulh instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_mulh mulh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulh_instruction)
		, .operand_rs2(operand_rs2_mulh)
		, .operand_rs1(operand_rs1_mulh)
		, .operand_rd(operand_rd_mulh)
	);

	// Instance for mulhsu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_mulhsu mulhsu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulhsu_instruction)
		, .operand_rs2(operand_rs2_mulhsu)
		, .operand_rs1(operand_rs1_mulhsu)
		, .operand_rd(operand_rd_mulhsu)
	);

	// Instance for mulhu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_mulhu mulhu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulhu_instruction)
		, .operand_rs2(operand_rs2_mulhu)
		, .operand_rs1(operand_rs1_mulhu)
		, .operand_rd(operand_rd_mulhu)
	);

	// Instance for div instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_div div_inst (
		  .opcode(opcode)
		, .opcode_valid(is_div_instruction)
		, .operand_rs2(operand_rs2_div)
		, .operand_rs1(operand_rs1_div)
		, .operand_rd(operand_rd_div)
	);

	// Instance for divu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_divu divu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_divu_instruction)
		, .operand_rs2(operand_rs2_divu)
		, .operand_rs1(operand_rs1_divu)
		, .operand_rd(operand_rd_divu)
	);

	// Instance for rem instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_rem rem_inst (
		  .opcode(opcode)
		, .opcode_valid(is_rem_instruction)
		, .operand_rs2(operand_rs2_rem)
		, .operand_rs1(operand_rs1_rem)
		, .operand_rd(operand_rd_rem)
	);

	// Instance for remu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_remu remu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_remu_instruction)
		, .operand_rs2(operand_rs2_remu)
		, .operand_rs1(operand_rs1_remu)
		, .operand_rd(operand_rd_remu)
	);

	// Instance for csrrw instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrw csrrw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrw_instruction)
		, .operand_csr(operand_csr_csrrw)
		, .operand_rs1(operand_rs1_csrrw)
		, .operand_rd(operand_rd_csrrw)
	);

	// Instance for csrrs instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrs csrrs_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrs_instruction)
		, .operand_csr(operand_csr_csrrs)
		, .operand_rs1(operand_rs1_csrrs)
		, .operand_rd(operand_rd_csrrs)
	);

	// Instance for csrrc instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrc csrrc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrc_instruction)
		, .operand_csr(operand_csr_csrrc)
		, .operand_rs1(operand_rs1_csrrc)
		, .operand_rd(operand_rd_csrrc)
	);

	// Instance for csrrwi instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrwi csrrwi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrwi_instruction)
		, .operand_csr(operand_csr_csrrwi)
		, .operand_zimm(operand_zimm_csrrwi)
		, .operand_rd(operand_rd_csrrwi)
	);

	// Instance for csrrsi instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrsi csrrsi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrsi_instruction)
		, .operand_csr(operand_csr_csrrsi)
		, .operand_zimm(operand_zimm_csrrsi)
		, .operand_rd(operand_rd_csrrsi)
	);

	// Instance for csrrci instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_csrrci csrrci_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrci_instruction)
		, .operand_csr(operand_csr_csrrci)
		, .operand_zimm(operand_zimm_csrrci)
		, .operand_rd(operand_rd_csrrci)
	);

	// Instance for p_extract instruction
	// Operands [ls3 (5 bits), ls2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extract p_extract_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extract_instruction)
		, .operand_ls3(operand_ls3_p_extract)
		, .operand_ls2(operand_ls2_p_extract)
		, .operand_rs1(operand_rs1_p_extract)
		, .operand_rd(operand_rd_p_extract)
	);

	// Instance for p_extractu instruction
	// Operands [ls3 (5 bits), ls2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extractu p_extractu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extractu_instruction)
		, .operand_ls3(operand_ls3_p_extractu)
		, .operand_ls2(operand_ls2_p_extractu)
		, .operand_rs1(operand_rs1_p_extractu)
		, .operand_rd(operand_rd_p_extractu)
	);

	// Instance for p_insert instruction
	// Operands [ls3 (5 bits), ls2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_insert p_insert_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_insert_instruction)
		, .operand_ls3(operand_ls3_p_insert)
		, .operand_ls2(operand_ls2_p_insert)
		, .operand_rs1(operand_rs1_p_insert)
		, .operand_rd(operand_rd_p_insert)
	);

	// Instance for p_bclr instruction
	// Operands [ls3 (5 bits), ls2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_bclr p_bclr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_bclr_instruction)
		, .operand_ls3(operand_ls3_p_bclr)
		, .operand_ls2(operand_ls2_p_bclr)
		, .operand_rs1(operand_rs1_p_bclr)
		, .operand_rd(operand_rd_p_bclr)
	);

	// Instance for p_bset instruction
	// Operands [ls3 (5 bits), ls2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_bset p_bset_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_bset_instruction)
		, .operand_ls3(operand_ls3_p_bset)
		, .operand_ls2(operand_ls2_p_bset)
		, .operand_rs1(operand_rs1_p_bset)
		, .operand_rd(operand_rd_p_bset)
	);

	// Instance for p_extractr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extractr p_extractr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extractr_instruction)
		, .operand_rs2(operand_rs2_p_extractr)
		, .operand_rs1(operand_rs1_p_extractr)
		, .operand_rd(operand_rd_p_extractr)
	);

	// Instance for p_extractur instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extractur p_extractur_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extractur_instruction)
		, .operand_rs2(operand_rs2_p_extractur)
		, .operand_rs1(operand_rs1_p_extractur)
		, .operand_rd(operand_rd_p_extractur)
	);

	// Instance for p_insertr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_insertr p_insertr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_insertr_instruction)
		, .operand_rs2(operand_rs2_p_insertr)
		, .operand_rs1(operand_rs1_p_insertr)
		, .operand_rd(operand_rd_p_insertr)
	);

	// Instance for p_bclrr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_bclrr p_bclrr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_bclrr_instruction)
		, .operand_rs2(operand_rs2_p_bclrr)
		, .operand_rs1(operand_rs1_p_bclrr)
		, .operand_rd(operand_rd_p_bclrr)
	);

	// Instance for p_bsetr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_bsetr p_bsetr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_bsetr_instruction)
		, .operand_rs2(operand_rs2_p_bsetr)
		, .operand_rs1(operand_rs1_p_bsetr)
		, .operand_rd(operand_rd_p_bsetr)
	);

	// Instance for p_ror instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_ror p_ror_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_ror_instruction)
		, .operand_rs2(operand_rs2_p_ror)
		, .operand_rs1(operand_rs1_p_ror)
		, .operand_rd(operand_rd_p_ror)
	);

	// Instance for p_ff1 instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_ff1 p_ff1_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_ff1_instruction)
		, .operand_rs1(operand_rs1_p_ff1)
		, .operand_rd(operand_rd_p_ff1)
	);

	// Instance for p_fl1 instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_fl1 p_fl1_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_fl1_instruction)
		, .operand_rs1(operand_rs1_p_fl1)
		, .operand_rd(operand_rd_p_fl1)
	);

	// Instance for p_clb instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_clb p_clb_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_clb_instruction)
		, .operand_rs1(operand_rs1_p_clb)
		, .operand_rd(operand_rd_p_clb)
	);

	// Instance for p_cnt instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_cnt p_cnt_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_cnt_instruction)
		, .operand_rs1(operand_rs1_p_cnt)
		, .operand_rd(operand_rd_p_cnt)
	);

	// Instance for p_abs instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_abs p_abs_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_abs_instruction)
		, .operand_rs1(operand_rs1_p_abs)
		, .operand_rd(operand_rd_p_abs)
	);

	// Instance for p_slet instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_slet p_slet_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_slet_instruction)
		, .operand_rs2(operand_rs2_p_slet)
		, .operand_rs1(operand_rs1_p_slet)
		, .operand_rd(operand_rd_p_slet)
	);

	// Instance for p_sletu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sletu p_sletu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sletu_instruction)
		, .operand_rs2(operand_rs2_p_sletu)
		, .operand_rs1(operand_rs1_p_sletu)
		, .operand_rd(operand_rd_p_sletu)
	);

	// Instance for p_min instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_min p_min_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_min_instruction)
		, .operand_rs2(operand_rs2_p_min)
		, .operand_rs1(operand_rs1_p_min)
		, .operand_rd(operand_rd_p_min)
	);

	// Instance for p_minu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_minu p_minu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_minu_instruction)
		, .operand_rs2(operand_rs2_p_minu)
		, .operand_rs1(operand_rs1_p_minu)
		, .operand_rd(operand_rd_p_minu)
	);

	// Instance for p_max instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_max p_max_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_max_instruction)
		, .operand_rs2(operand_rs2_p_max)
		, .operand_rs1(operand_rs1_p_max)
		, .operand_rd(operand_rd_p_max)
	);

	// Instance for p_maxu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_maxu p_maxu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_maxu_instruction)
		, .operand_rs2(operand_rs2_p_maxu)
		, .operand_rs1(operand_rs1_p_maxu)
		, .operand_rd(operand_rd_p_maxu)
	);

	// Instance for p_exths instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_exths p_exths_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_exths_instruction)
		, .operand_rs1(operand_rs1_p_exths)
		, .operand_rd(operand_rd_p_exths)
	);

	// Instance for p_exthz instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_exthz p_exthz_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_exthz_instruction)
		, .operand_rs1(operand_rs1_p_exthz)
		, .operand_rd(operand_rd_p_exthz)
	);

	// Instance for p_extbs instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extbs p_extbs_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extbs_instruction)
		, .operand_rs1(operand_rs1_p_extbs)
		, .operand_rd(operand_rd_p_extbs)
	);

	// Instance for p_extbz instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_extbz p_extbz_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_extbz_instruction)
		, .operand_rs1(operand_rs1_p_extbz)
		, .operand_rd(operand_rd_p_extbz)
	);

	// Instance for p_clip instruction
	// Operands [uimm (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_clip p_clip_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_clip_instruction)
		, .operand_uimm(operand_uimm_p_clip)
		, .operand_rs1(operand_rs1_p_clip)
		, .operand_rd(operand_rd_p_clip)
	);

	// Instance for p_clipu instruction
	// Operands [uimm (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_clipu p_clipu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_clipu_instruction)
		, .operand_uimm(operand_uimm_p_clipu)
		, .operand_rs1(operand_rs1_p_clipu)
		, .operand_rd(operand_rd_p_clipu)
	);

	// Instance for p_clipr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_clipr p_clipr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_clipr_instruction)
		, .operand_rs2(operand_rs2_p_clipr)
		, .operand_rs1(operand_rs1_p_clipr)
		, .operand_rd(operand_rd_p_clipr)
	);

	// Instance for p_clipur instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_clipur p_clipur_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_clipur_instruction)
		, .operand_rs2(operand_rs2_p_clipur)
		, .operand_rs1(operand_rs1_p_clipur)
		, .operand_rd(operand_rd_p_clipur)
	);

	// Instance for p_addn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addn p_addn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addn_instruction)
		, .operand_uimm(operand_uimm_p_addn)
		, .operand_rs2(operand_rs2_p_addn)
		, .operand_rs1(operand_rs1_p_addn)
		, .operand_rd(operand_rd_p_addn)
	);

	// Instance for p_addun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addun p_addun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addun_instruction)
		, .operand_uimm(operand_uimm_p_addun)
		, .operand_rs2(operand_rs2_p_addun)
		, .operand_rs1(operand_rs1_p_addun)
		, .operand_rd(operand_rd_p_addun)
	);

	// Instance for p_addrn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addrn p_addrn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addrn_instruction)
		, .operand_uimm(operand_uimm_p_addrn)
		, .operand_rs2(operand_rs2_p_addrn)
		, .operand_rs1(operand_rs1_p_addrn)
		, .operand_rd(operand_rd_p_addrn)
	);

	// Instance for p_addurn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addurn p_addurn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addurn_instruction)
		, .operand_uimm(operand_uimm_p_addurn)
		, .operand_rs2(operand_rs2_p_addurn)
		, .operand_rs1(operand_rs1_p_addurn)
		, .operand_rd(operand_rd_p_addurn)
	);

	// Instance for p_subn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subn p_subn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subn_instruction)
		, .operand_uimm(operand_uimm_p_subn)
		, .operand_rs2(operand_rs2_p_subn)
		, .operand_rs1(operand_rs1_p_subn)
		, .operand_rd(operand_rd_p_subn)
	);

	// Instance for p_subun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subun p_subun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subun_instruction)
		, .operand_uimm(operand_uimm_p_subun)
		, .operand_rs2(operand_rs2_p_subun)
		, .operand_rs1(operand_rs1_p_subun)
		, .operand_rd(operand_rd_p_subun)
	);

	// Instance for p_subrn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subrn p_subrn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subrn_instruction)
		, .operand_uimm(operand_uimm_p_subrn)
		, .operand_rs2(operand_rs2_p_subrn)
		, .operand_rs1(operand_rs1_p_subrn)
		, .operand_rd(operand_rd_p_subrn)
	);

	// Instance for p_suburn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_suburn p_suburn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_suburn_instruction)
		, .operand_uimm(operand_uimm_p_suburn)
		, .operand_rs2(operand_rs2_p_suburn)
		, .operand_rs1(operand_rs1_p_suburn)
		, .operand_rd(operand_rd_p_suburn)
	);

	// Instance for p_addnr instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addnr p_addnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addnr_instruction)
		, .operand_uimm(operand_uimm_p_addnr)
		, .operand_rs2(operand_rs2_p_addnr)
		, .operand_rs1(operand_rs1_p_addnr)
		, .operand_rd(operand_rd_p_addnr)
	);

	// Instance for p_addunr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addunr p_addunr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addunr_instruction)
		, .operand_rs2(operand_rs2_p_addunr)
		, .operand_rs1(operand_rs1_p_addunr)
		, .operand_rd(operand_rd_p_addunr)
	);

	// Instance for p_addrnr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addrnr p_addrnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addrnr_instruction)
		, .operand_rs2(operand_rs2_p_addrnr)
		, .operand_rs1(operand_rs1_p_addrnr)
		, .operand_rd(operand_rd_p_addrnr)
	);

	// Instance for p_addurnr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_addurnr p_addurnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_addurnr_instruction)
		, .operand_rs2(operand_rs2_p_addurnr)
		, .operand_rs1(operand_rs1_p_addurnr)
		, .operand_rd(operand_rd_p_addurnr)
	);

	// Instance for p_subnr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subnr p_subnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subnr_instruction)
		, .operand_rs2(operand_rs2_p_subnr)
		, .operand_rs1(operand_rs1_p_subnr)
		, .operand_rd(operand_rd_p_subnr)
	);

	// Instance for p_subunr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subunr p_subunr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subunr_instruction)
		, .operand_rs2(operand_rs2_p_subunr)
		, .operand_rs1(operand_rs1_p_subunr)
		, .operand_rd(operand_rd_p_subunr)
	);

	// Instance for p_subrnr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_subrnr p_subrnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_subrnr_instruction)
		, .operand_rs2(operand_rs2_p_subrnr)
		, .operand_rs1(operand_rs1_p_subrnr)
		, .operand_rd(operand_rd_p_subrnr)
	);

	// Instance for p_suburnr instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_suburnr p_suburnr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_suburnr_instruction)
		, .operand_rs2(operand_rs2_p_suburnr)
		, .operand_rs1(operand_rs1_p_suburnr)
		, .operand_rd(operand_rd_p_suburnr)
	);

	// Instance for lp_starti instruction
	// Operands [uimmL (12 bits), L (1 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_starti lp_starti_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_starti_instruction)
		, .operand_uimmL(operand_uimmL_lp_starti)
		, .operand_L(operand_L_lp_starti)
	);

	// Instance for lp_endi instruction
	// Operands [uimmL (12 bits), L (1 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_endi lp_endi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_endi_instruction)
		, .operand_uimmL(operand_uimmL_lp_endi)
		, .operand_L(operand_L_lp_endi)
	);

	// Instance for lp_counti instruction
	// Operands [uimmL (12 bits), L (1 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_counti lp_counti_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_counti_instruction)
		, .operand_uimmL(operand_uimmL_lp_counti)
		, .operand_L(operand_L_lp_counti)
	);

	// Instance for lp_count instruction
	// Operands [rs1 (5 bits), L (1 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_count lp_count_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_count_instruction)
		, .operand_rs1(operand_rs1_lp_count)
		, .operand_L(operand_L_lp_count)
	);

	// Instance for lp_setup instruction
	// Operands [uimmL (12 bits), rs1 (5 bits), L (1 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_setup lp_setup_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_setup_instruction)
		, .operand_uimmL(operand_uimmL_lp_setup)
		, .operand_rs1(operand_rs1_lp_setup)
		, .operand_L(operand_L_lp_setup)
	);

	// Instance for lp_setupi instruction
	// Operands [uimmL (12 bits), uimmS (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_lp_setupi lp_setupi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lp_setupi_instruction)
		, .operand_uimmL(operand_uimmL_lp_setupi)
		, .operand_uimmS(operand_uimmS_lp_setupi)
	);

	// Instance for p_beqimm instruction
	// Operands [imm1 (12 bits), imm2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_beqimm p_beqimm_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_beqimm_instruction)
		, .operand_imm1(operand_imm1_p_beqimm)
		, .operand_imm2(operand_imm2_p_beqimm)
		, .operand_rs1(operand_rs1_p_beqimm)
	);

	// Instance for p_bneimm instruction
	// Operands [imm1 (12 bits), imm2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_bneimm p_bneimm_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_bneimm_instruction)
		, .operand_imm1(operand_imm1_p_bneimm)
		, .operand_imm2(operand_imm2_p_bneimm)
		, .operand_rs1(operand_rs1_p_bneimm)
	);

	// Instance for p_lb_i_inc instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lb_i_inc p_lb_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lb_i_inc_instruction)
		, .operand_imm(operand_imm_p_lb_i_inc)
		, .operand_rs1(operand_rs1_p_lb_i_inc)
		, .operand_rd(operand_rd_p_lb_i_inc)
	);

	// Instance for p_lbu_i_inc instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lbu_i_inc p_lbu_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lbu_i_inc_instruction)
		, .operand_imm(operand_imm_p_lbu_i_inc)
		, .operand_rs1(operand_rs1_p_lbu_i_inc)
		, .operand_rd(operand_rd_p_lbu_i_inc)
	);

	// Instance for p_lh_i_inc instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lh_i_inc p_lh_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lh_i_inc_instruction)
		, .operand_imm(operand_imm_p_lh_i_inc)
		, .operand_rs1(operand_rs1_p_lh_i_inc)
		, .operand_rd(operand_rd_p_lh_i_inc)
	);

	// Instance for p_lhu_i_inc instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lhu_i_inc p_lhu_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lhu_i_inc_instruction)
		, .operand_imm(operand_imm_p_lhu_i_inc)
		, .operand_rs1(operand_rs1_p_lhu_i_inc)
		, .operand_rd(operand_rd_p_lhu_i_inc)
	);

	// Instance for p_lw_i_inc instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lw_i_inc p_lw_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lw_i_inc_instruction)
		, .operand_imm(operand_imm_p_lw_i_inc)
		, .operand_rs1(operand_rs1_p_lw_i_inc)
		, .operand_rd(operand_rd_p_lw_i_inc)
	);

	// Instance for p_lb_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lb_r_inc p_lb_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lb_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_lb_r_inc)
		, .operand_rs1(operand_rs1_p_lb_r_inc)
		, .operand_rd(operand_rd_p_lb_r_inc)
	);

	// Instance for p_lbu_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lbu_r_inc p_lbu_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lbu_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_lbu_r_inc)
		, .operand_rs1(operand_rs1_p_lbu_r_inc)
		, .operand_rd(operand_rd_p_lbu_r_inc)
	);

	// Instance for p_lh_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lh_r_inc p_lh_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lh_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_lh_r_inc)
		, .operand_rs1(operand_rs1_p_lh_r_inc)
		, .operand_rd(operand_rd_p_lh_r_inc)
	);

	// Instance for p_lhu_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lhu_r_inc p_lhu_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lhu_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_lhu_r_inc)
		, .operand_rs1(operand_rs1_p_lhu_r_inc)
		, .operand_rd(operand_rd_p_lhu_r_inc)
	);

	// Instance for p_lw_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lw_r_inc p_lw_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lw_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_lw_r_inc)
		, .operand_rs1(operand_rs1_p_lw_r_inc)
		, .operand_rd(operand_rd_p_lw_r_inc)
	);

	// Instance for p_lb_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lb_r p_lb_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lb_r_instruction)
		, .operand_rs2(operand_rs2_p_lb_r)
		, .operand_rs1(operand_rs1_p_lb_r)
		, .operand_rd(operand_rd_p_lb_r)
	);

	// Instance for p_lbu_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lbu_r p_lbu_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lbu_r_instruction)
		, .operand_rs2(operand_rs2_p_lbu_r)
		, .operand_rs1(operand_rs1_p_lbu_r)
		, .operand_rd(operand_rd_p_lbu_r)
	);

	// Instance for p_lh_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lh_r p_lh_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lh_r_instruction)
		, .operand_rs2(operand_rs2_p_lh_r)
		, .operand_rs1(operand_rs1_p_lh_r)
		, .operand_rd(operand_rd_p_lh_r)
	);

	// Instance for p_lhu_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lhu_r p_lhu_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lhu_r_instruction)
		, .operand_rs2(operand_rs2_p_lhu_r)
		, .operand_rs1(operand_rs1_p_lhu_r)
		, .operand_rd(operand_rd_p_lhu_r)
	);

	// Instance for p_lw_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_lw_r p_lw_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_lw_r_instruction)
		, .operand_rs2(operand_rs2_p_lw_r)
		, .operand_rs1(operand_rs1_p_lw_r)
		, .operand_rd(operand_rd_p_lw_r)
	);

	// Instance for p_mulu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulu p_mulu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulu_instruction)
		, .operand_rs2(operand_rs2_p_mulu)
		, .operand_rs1(operand_rs1_p_mulu)
		, .operand_rd(operand_rd_p_mulu)
	);

	// Instance for p_mulhhu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulhhu p_mulhhu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulhhu_instruction)
		, .operand_rs2(operand_rs2_p_mulhhu)
		, .operand_rs1(operand_rs1_p_mulhhu)
		, .operand_rd(operand_rd_p_mulhhu)
	);

	// Instance for p_mulun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulun p_mulun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulun_instruction)
		, .operand_uimm(operand_uimm_p_mulun)
		, .operand_rs2(operand_rs2_p_mulun)
		, .operand_rs1(operand_rs1_p_mulun)
		, .operand_rd(operand_rd_p_mulun)
	);

	// Instance for p_mulhhun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulhhun p_mulhhun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulhhun_instruction)
		, .operand_uimm(operand_uimm_p_mulhhun)
		, .operand_rs2(operand_rs2_p_mulhhun)
		, .operand_rs1(operand_rs1_p_mulhhun)
		, .operand_rd(operand_rd_p_mulhhun)
	);

	// Instance for p_mulurn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulurn p_mulurn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulurn_instruction)
		, .operand_uimm(operand_uimm_p_mulurn)
		, .operand_rs2(operand_rs2_p_mulurn)
		, .operand_rs1(operand_rs1_p_mulurn)
		, .operand_rd(operand_rd_p_mulurn)
	);

	// Instance for p_mulhhurn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_mulhhurn p_mulhhurn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_mulhhurn_instruction)
		, .operand_uimm(operand_uimm_p_mulhhurn)
		, .operand_rs2(operand_rs2_p_mulhhurn)
		, .operand_rs1(operand_rs1_p_mulhhurn)
		, .operand_rd(operand_rd_p_mulhhurn)
	);

	// Instance for p_macsn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_macsn p_macsn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_macsn_instruction)
		, .operand_uimm(operand_uimm_p_macsn)
		, .operand_rs2(operand_rs2_p_macsn)
		, .operand_rs1(operand_rs1_p_macsn)
		, .operand_rd(operand_rd_p_macsn)
	);

	// Instance for p_machhsn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_machhsn p_machhsn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_machhsn_instruction)
		, .operand_uimm(operand_uimm_p_machhsn)
		, .operand_rs2(operand_rs2_p_machhsn)
		, .operand_rs1(operand_rs1_p_machhsn)
		, .operand_rd(operand_rd_p_machhsn)
	);

	// Instance for p_macsrn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_macsrn p_macsrn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_macsrn_instruction)
		, .operand_uimm(operand_uimm_p_macsrn)
		, .operand_rs2(operand_rs2_p_macsrn)
		, .operand_rs1(operand_rs1_p_macsrn)
		, .operand_rd(operand_rd_p_macsrn)
	);

	// Instance for p_machhsrn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_machhsrn p_machhsrn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_machhsrn_instruction)
		, .operand_uimm(operand_uimm_p_machhsrn)
		, .operand_rs2(operand_rs2_p_machhsrn)
		, .operand_rs1(operand_rs1_p_machhsrn)
		, .operand_rd(operand_rd_p_machhsrn)
	);

	// Instance for p_macun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_macun p_macun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_macun_instruction)
		, .operand_uimm(operand_uimm_p_macun)
		, .operand_rs2(operand_rs2_p_macun)
		, .operand_rs1(operand_rs1_p_macun)
		, .operand_rd(operand_rd_p_macun)
	);

	// Instance for p_machhun instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_machhun p_machhun_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_machhun_instruction)
		, .operand_uimm(operand_uimm_p_machhun)
		, .operand_rs2(operand_rs2_p_machhun)
		, .operand_rs1(operand_rs1_p_machhun)
		, .operand_rd(operand_rd_p_machhun)
	);

	// Instance for p_macurn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_macurn p_macurn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_macurn_instruction)
		, .operand_uimm(operand_uimm_p_macurn)
		, .operand_rs2(operand_rs2_p_macurn)
		, .operand_rs1(operand_rs1_p_macurn)
		, .operand_rd(operand_rd_p_macurn)
	);

	// Instance for p_machhurn instruction
	// Operands [uimm (5 bits), rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_machhurn p_machhurn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_machhurn_instruction)
		, .operand_uimm(operand_uimm_p_machhurn)
		, .operand_rs2(operand_rs2_p_machhurn)
		, .operand_rs1(operand_rs1_p_machhurn)
		, .operand_rd(operand_rd_p_machhurn)
	);

	// Instance for p_sb_i_inc instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sb_i_inc p_sb_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sb_i_inc_instruction)
		, .operand_imm(operand_imm_p_sb_i_inc)
		, .operand_rs2(operand_rs2_p_sb_i_inc)
		, .operand_rs1(operand_rs1_p_sb_i_inc)
	);

	// Instance for p_sh_i_inc instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sh_i_inc p_sh_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sh_i_inc_instruction)
		, .operand_imm(operand_imm_p_sh_i_inc)
		, .operand_rs2(operand_rs2_p_sh_i_inc)
		, .operand_rs1(operand_rs1_p_sh_i_inc)
	);

	// Instance for p_sw_i_inc instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sw_i_inc p_sw_i_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sw_i_inc_instruction)
		, .operand_imm(operand_imm_p_sw_i_inc)
		, .operand_rs2(operand_rs2_p_sw_i_inc)
		, .operand_rs1(operand_rs1_p_sw_i_inc)
	);

	// Instance for p_sb_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sb_r_inc p_sb_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sb_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_sb_r_inc)
		, .operand_rs1(operand_rs1_p_sb_r_inc)
		, .operand_rd(operand_rd_p_sb_r_inc)
	);

	// Instance for p_sh_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sh_r_inc p_sh_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sh_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_sh_r_inc)
		, .operand_rs1(operand_rs1_p_sh_r_inc)
		, .operand_rd(operand_rd_p_sh_r_inc)
	);

	// Instance for p_sw_r_inc instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sw_r_inc p_sw_r_inc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sw_r_inc_instruction)
		, .operand_rs2(operand_rs2_p_sw_r_inc)
		, .operand_rs1(operand_rs1_p_sw_r_inc)
		, .operand_rd(operand_rd_p_sw_r_inc)
	);

	// Instance for p_sb_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sb_r p_sb_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sb_r_instruction)
		, .operand_rs2(operand_rs2_p_sb_r)
		, .operand_rs1(operand_rs1_p_sb_r)
		, .operand_rd(operand_rd_p_sb_r)
	);

	// Instance for p_sh_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sh_r p_sh_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sh_r_instruction)
		, .operand_rs2(operand_rs2_p_sh_r)
		, .operand_rs1(operand_rs1_p_sh_r)
		, .operand_rd(operand_rd_p_sh_r)
	);

	// Instance for p_sw_r instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_p_sw_r p_sw_r_inst (
		  .opcode(opcode)
		, .opcode_valid(is_p_sw_r_instruction)
		, .operand_rs2(operand_rs2_p_sw_r)
		, .operand_rs1(operand_rs1_p_sw_r)
		, .operand_rd(operand_rd_p_sw_r)
	);

	// Instance for pv_cmpeq_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_h pv_cmpeq_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpeq_h)
		, .operand_rs1(operand_rs1_pv_cmpeq_h)
		, .operand_rd(operand_rd_pv_cmpeq_h)
	);

	// Instance for pv_cmpeq_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sc_h pv_cmpeq_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpeq_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpeq_sc_h)
		, .operand_rd(operand_rd_pv_cmpeq_sc_h)
	);

	// Instance for pv_cmpeq_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sci_h pv_cmpeq_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpeq_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpeq_sci_h)
		, .operand_rd(operand_rd_pv_cmpeq_sci_h)
	);

	// Instance for pv_cmpeq_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_b pv_cmpeq_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpeq_b)
		, .operand_rs1(operand_rs1_pv_cmpeq_b)
		, .operand_rd(operand_rd_pv_cmpeq_b)
	);

	// Instance for pv_cmpeq_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sc_b pv_cmpeq_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpeq_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpeq_sc_b)
		, .operand_rd(operand_rd_pv_cmpeq_sc_b)
	);

	// Instance for pv_cmpeq_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpeq_sci_b pv_cmpeq_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpeq_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpeq_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpeq_sci_b)
		, .operand_rd(operand_rd_pv_cmpeq_sci_b)
	);

	// Instance for pv_cmpne_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_h pv_cmpne_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpne_h)
		, .operand_rs1(operand_rs1_pv_cmpne_h)
		, .operand_rd(operand_rd_pv_cmpne_h)
	);

	// Instance for pv_cmpne_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sc_h pv_cmpne_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpne_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpne_sc_h)
		, .operand_rd(operand_rd_pv_cmpne_sc_h)
	);

	// Instance for pv_cmpne_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sci_h pv_cmpne_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpne_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpne_sci_h)
		, .operand_rd(operand_rd_pv_cmpne_sci_h)
	);

	// Instance for pv_cmpne_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_b pv_cmpne_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpne_b)
		, .operand_rs1(operand_rs1_pv_cmpne_b)
		, .operand_rd(operand_rd_pv_cmpne_b)
	);

	// Instance for pv_cmpne_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sc_b pv_cmpne_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpne_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpne_sc_b)
		, .operand_rd(operand_rd_pv_cmpne_sc_b)
	);

	// Instance for pv_cmpne_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpne_sci_b pv_cmpne_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpne_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpne_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpne_sci_b)
		, .operand_rd(operand_rd_pv_cmpne_sci_b)
	);

	// Instance for pv_cmpgt_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_h pv_cmpgt_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgt_h)
		, .operand_rs1(operand_rs1_pv_cmpgt_h)
		, .operand_rd(operand_rd_pv_cmpgt_h)
	);

	// Instance for pv_cmpgt_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sc_h pv_cmpgt_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgt_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpgt_sc_h)
		, .operand_rd(operand_rd_pv_cmpgt_sc_h)
	);

	// Instance for pv_cmpgt_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sci_h pv_cmpgt_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpgt_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpgt_sci_h)
		, .operand_rd(operand_rd_pv_cmpgt_sci_h)
	);

	// Instance for pv_cmpgt_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_b pv_cmpgt_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgt_b)
		, .operand_rs1(operand_rs1_pv_cmpgt_b)
		, .operand_rd(operand_rd_pv_cmpgt_b)
	);

	// Instance for pv_cmpgt_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sc_b pv_cmpgt_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgt_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpgt_sc_b)
		, .operand_rd(operand_rd_pv_cmpgt_sc_b)
	);

	// Instance for pv_cmpgt_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgt_sci_b pv_cmpgt_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgt_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpgt_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpgt_sci_b)
		, .operand_rd(operand_rd_pv_cmpgt_sci_b)
	);

	// Instance for pv_cmpge_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_h pv_cmpge_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpge_h)
		, .operand_rs1(operand_rs1_pv_cmpge_h)
		, .operand_rd(operand_rd_pv_cmpge_h)
	);

	// Instance for pv_cmpge_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sc_h pv_cmpge_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpge_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpge_sc_h)
		, .operand_rd(operand_rd_pv_cmpge_sc_h)
	);

	// Instance for pv_cmpge_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sci_h pv_cmpge_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpge_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpge_sci_h)
		, .operand_rd(operand_rd_pv_cmpge_sci_h)
	);

	// Instance for pv_cmpge_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_b pv_cmpge_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpge_b)
		, .operand_rs1(operand_rs1_pv_cmpge_b)
		, .operand_rd(operand_rd_pv_cmpge_b)
	);

	// Instance for pv_cmpge_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sc_b pv_cmpge_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpge_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpge_sc_b)
		, .operand_rd(operand_rd_pv_cmpge_sc_b)
	);

	// Instance for pv_cmpge_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpge_sci_b pv_cmpge_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpge_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpge_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpge_sci_b)
		, .operand_rd(operand_rd_pv_cmpge_sci_b)
	);

	// Instance for pv_cmplt_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_h pv_cmplt_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmplt_h)
		, .operand_rs1(operand_rs1_pv_cmplt_h)
		, .operand_rd(operand_rd_pv_cmplt_h)
	);

	// Instance for pv_cmplt_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sc_h pv_cmplt_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmplt_sc_h)
		, .operand_rs1(operand_rs1_pv_cmplt_sc_h)
		, .operand_rd(operand_rd_pv_cmplt_sc_h)
	);

	// Instance for pv_cmplt_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sci_h pv_cmplt_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmplt_sci_h)
		, .operand_rs1(operand_rs1_pv_cmplt_sci_h)
		, .operand_rd(operand_rd_pv_cmplt_sci_h)
	);

	// Instance for pv_cmplt_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_b pv_cmplt_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmplt_b)
		, .operand_rs1(operand_rs1_pv_cmplt_b)
		, .operand_rd(operand_rd_pv_cmplt_b)
	);

	// Instance for pv_cmplt_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sc_b pv_cmplt_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmplt_sc_b)
		, .operand_rs1(operand_rs1_pv_cmplt_sc_b)
		, .operand_rd(operand_rd_pv_cmplt_sc_b)
	);

	// Instance for pv_cmplt_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmplt_sci_b pv_cmplt_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmplt_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmplt_sci_b)
		, .operand_rs1(operand_rs1_pv_cmplt_sci_b)
		, .operand_rd(operand_rd_pv_cmplt_sci_b)
	);

	// Instance for pv_cmple_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_h pv_cmple_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmple_h)
		, .operand_rs1(operand_rs1_pv_cmple_h)
		, .operand_rd(operand_rd_pv_cmple_h)
	);

	// Instance for pv_cmple_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sc_h pv_cmple_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmple_sc_h)
		, .operand_rs1(operand_rs1_pv_cmple_sc_h)
		, .operand_rd(operand_rd_pv_cmple_sc_h)
	);

	// Instance for pv_cmple_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sci_h pv_cmple_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmple_sci_h)
		, .operand_rs1(operand_rs1_pv_cmple_sci_h)
		, .operand_rd(operand_rd_pv_cmple_sci_h)
	);

	// Instance for pv_cmple_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_b pv_cmple_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmple_b)
		, .operand_rs1(operand_rs1_pv_cmple_b)
		, .operand_rd(operand_rd_pv_cmple_b)
	);

	// Instance for pv_cmple_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sc_b pv_cmple_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmple_sc_b)
		, .operand_rs1(operand_rs1_pv_cmple_sc_b)
		, .operand_rd(operand_rd_pv_cmple_sc_b)
	);

	// Instance for pv_cmple_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmple_sci_b pv_cmple_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmple_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmple_sci_b)
		, .operand_rs1(operand_rs1_pv_cmple_sci_b)
		, .operand_rd(operand_rd_pv_cmple_sci_b)
	);

	// Instance for pv_cmpgtu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_h pv_cmpgtu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgtu_h)
		, .operand_rs1(operand_rs1_pv_cmpgtu_h)
		, .operand_rd(operand_rd_pv_cmpgtu_h)
	);

	// Instance for pv_cmpgtu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sc_h pv_cmpgtu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgtu_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpgtu_sc_h)
		, .operand_rd(operand_rd_pv_cmpgtu_sc_h)
	);

	// Instance for pv_cmpgtu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sci_h pv_cmpgtu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpgtu_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpgtu_sci_h)
		, .operand_rd(operand_rd_pv_cmpgtu_sci_h)
	);

	// Instance for pv_cmpgtu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_b pv_cmpgtu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgtu_b)
		, .operand_rs1(operand_rs1_pv_cmpgtu_b)
		, .operand_rd(operand_rd_pv_cmpgtu_b)
	);

	// Instance for pv_cmpgtu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sc_b pv_cmpgtu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgtu_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpgtu_sc_b)
		, .operand_rd(operand_rd_pv_cmpgtu_sc_b)
	);

	// Instance for pv_cmpgtu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgtu_sci_b pv_cmpgtu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgtu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpgtu_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpgtu_sci_b)
		, .operand_rd(operand_rd_pv_cmpgtu_sci_b)
	);

	// Instance for pv_cmpgeu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_h pv_cmpgeu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgeu_h)
		, .operand_rs1(operand_rs1_pv_cmpgeu_h)
		, .operand_rd(operand_rd_pv_cmpgeu_h)
	);

	// Instance for pv_cmpgeu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sc_h pv_cmpgeu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgeu_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpgeu_sc_h)
		, .operand_rd(operand_rd_pv_cmpgeu_sc_h)
	);

	// Instance for pv_cmpgeu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sci_h pv_cmpgeu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpgeu_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpgeu_sci_h)
		, .operand_rd(operand_rd_pv_cmpgeu_sci_h)
	);

	// Instance for pv_cmpgeu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_b pv_cmpgeu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgeu_b)
		, .operand_rs1(operand_rs1_pv_cmpgeu_b)
		, .operand_rd(operand_rd_pv_cmpgeu_b)
	);

	// Instance for pv_cmpgeu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sc_b pv_cmpgeu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpgeu_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpgeu_sc_b)
		, .operand_rd(operand_rd_pv_cmpgeu_sc_b)
	);

	// Instance for pv_cmpgeu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpgeu_sci_b pv_cmpgeu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpgeu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpgeu_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpgeu_sci_b)
		, .operand_rd(operand_rd_pv_cmpgeu_sci_b)
	);

	// Instance for pv_cmpltu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_h pv_cmpltu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpltu_h)
		, .operand_rs1(operand_rs1_pv_cmpltu_h)
		, .operand_rd(operand_rd_pv_cmpltu_h)
	);

	// Instance for pv_cmpltu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sc_h pv_cmpltu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpltu_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpltu_sc_h)
		, .operand_rd(operand_rd_pv_cmpltu_sc_h)
	);

	// Instance for pv_cmpltu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sci_h pv_cmpltu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpltu_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpltu_sci_h)
		, .operand_rd(operand_rd_pv_cmpltu_sci_h)
	);

	// Instance for pv_cmpltu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_b pv_cmpltu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpltu_b)
		, .operand_rs1(operand_rs1_pv_cmpltu_b)
		, .operand_rd(operand_rd_pv_cmpltu_b)
	);

	// Instance for pv_cmpltu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sc_b pv_cmpltu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpltu_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpltu_sc_b)
		, .operand_rd(operand_rd_pv_cmpltu_sc_b)
	);

	// Instance for pv_cmpltu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpltu_sci_b pv_cmpltu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpltu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpltu_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpltu_sci_b)
		, .operand_rd(operand_rd_pv_cmpltu_sci_b)
	);

	// Instance for pv_cmpleu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_h pv_cmpleu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpleu_h)
		, .operand_rs1(operand_rs1_pv_cmpleu_h)
		, .operand_rd(operand_rd_pv_cmpleu_h)
	);

	// Instance for pv_cmpleu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sc_h pv_cmpleu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_cmpleu_sc_h)
		, .operand_rs1(operand_rs1_pv_cmpleu_sc_h)
		, .operand_rd(operand_rd_pv_cmpleu_sc_h)
	);

	// Instance for pv_cmpleu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sci_h pv_cmpleu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_cmpleu_sci_h)
		, .operand_rs1(operand_rs1_pv_cmpleu_sci_h)
		, .operand_rd(operand_rd_pv_cmpleu_sci_h)
	);

	// Instance for pv_cmpleu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_b pv_cmpleu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpleu_b)
		, .operand_rs1(operand_rs1_pv_cmpleu_b)
		, .operand_rd(operand_rd_pv_cmpleu_b)
	);

	// Instance for pv_cmpleu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sc_b pv_cmpleu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_cmpleu_sc_b)
		, .operand_rs1(operand_rs1_pv_cmpleu_sc_b)
		, .operand_rd(operand_rd_pv_cmpleu_sc_b)
	);

	// Instance for pv_cmpleu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_cmpleu_sci_b pv_cmpleu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_cmpleu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_cmpleu_sci_b)
		, .operand_rs1(operand_rs1_pv_cmpleu_sci_b)
		, .operand_rd(operand_rd_pv_cmpleu_sci_b)
	);

	// Instance for pv_add_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_h pv_add_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_h_instruction)
		, .operand_rs2(operand_rs2_pv_add_h)
		, .operand_rs1(operand_rs1_pv_add_h)
		, .operand_rd(operand_rd_pv_add_h)
	);

	// Instance for pv_add_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_sc_h pv_add_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_add_sc_h)
		, .operand_rs1(operand_rs1_pv_add_sc_h)
		, .operand_rd(operand_rd_pv_add_sc_h)
	);

	// Instance for pv_add_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_sci_h pv_add_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_sci_h_instruction)
		, .operand_imm(operand_imm_pv_add_sci_h)
		, .operand_rs1(operand_rs1_pv_add_sci_h)
		, .operand_rd(operand_rd_pv_add_sci_h)
	);

	// Instance for pv_add_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_b pv_add_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_b_instruction)
		, .operand_rs2(operand_rs2_pv_add_b)
		, .operand_rs1(operand_rs1_pv_add_b)
		, .operand_rd(operand_rd_pv_add_b)
	);

	// Instance for pv_add_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_sc_b pv_add_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_add_sc_b)
		, .operand_rs1(operand_rs1_pv_add_sc_b)
		, .operand_rd(operand_rd_pv_add_sc_b)
	);

	// Instance for pv_add_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_add_sci_b pv_add_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_add_sci_b_instruction)
		, .operand_imm(operand_imm_pv_add_sci_b)
		, .operand_rs1(operand_rs1_pv_add_sci_b)
		, .operand_rd(operand_rd_pv_add_sci_b)
	);

	// Instance for pv_sub_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_h pv_sub_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_h_instruction)
		, .operand_rs2(operand_rs2_pv_sub_h)
		, .operand_rs1(operand_rs1_pv_sub_h)
		, .operand_rd(operand_rd_pv_sub_h)
	);

	// Instance for pv_sub_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_sc_h pv_sub_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sub_sc_h)
		, .operand_rs1(operand_rs1_pv_sub_sc_h)
		, .operand_rd(operand_rd_pv_sub_sc_h)
	);

	// Instance for pv_sub_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_sci_h pv_sub_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sub_sci_h)
		, .operand_rs1(operand_rs1_pv_sub_sci_h)
		, .operand_rd(operand_rd_pv_sub_sci_h)
	);

	// Instance for pv_sub_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_b pv_sub_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_b_instruction)
		, .operand_rs2(operand_rs2_pv_sub_b)
		, .operand_rs1(operand_rs1_pv_sub_b)
		, .operand_rd(operand_rd_pv_sub_b)
	);

	// Instance for pv_sub_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_sc_b pv_sub_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sub_sc_b)
		, .operand_rs1(operand_rs1_pv_sub_sc_b)
		, .operand_rd(operand_rd_pv_sub_sc_b)
	);

	// Instance for pv_sub_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sub_sci_b pv_sub_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sub_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sub_sci_b)
		, .operand_rs1(operand_rs1_pv_sub_sci_b)
		, .operand_rd(operand_rd_pv_sub_sci_b)
	);

	// Instance for pv_avg_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_h pv_avg_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_h_instruction)
		, .operand_rs2(operand_rs2_pv_avg_h)
		, .operand_rs1(operand_rs1_pv_avg_h)
		, .operand_rd(operand_rd_pv_avg_h)
	);

	// Instance for pv_avg_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_sc_h pv_avg_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_avg_sc_h)
		, .operand_rs1(operand_rs1_pv_avg_sc_h)
		, .operand_rd(operand_rd_pv_avg_sc_h)
	);

	// Instance for pv_avg_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_sci_h pv_avg_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_sci_h_instruction)
		, .operand_imm(operand_imm_pv_avg_sci_h)
		, .operand_rs1(operand_rs1_pv_avg_sci_h)
		, .operand_rd(operand_rd_pv_avg_sci_h)
	);

	// Instance for pv_avg_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_b pv_avg_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_b_instruction)
		, .operand_rs2(operand_rs2_pv_avg_b)
		, .operand_rs1(operand_rs1_pv_avg_b)
		, .operand_rd(operand_rd_pv_avg_b)
	);

	// Instance for pv_avg_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_sc_b pv_avg_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_avg_sc_b)
		, .operand_rs1(operand_rs1_pv_avg_sc_b)
		, .operand_rd(operand_rd_pv_avg_sc_b)
	);

	// Instance for pv_avg_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avg_sci_b pv_avg_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avg_sci_b_instruction)
		, .operand_imm(operand_imm_pv_avg_sci_b)
		, .operand_rs1(operand_rs1_pv_avg_sci_b)
		, .operand_rd(operand_rd_pv_avg_sci_b)
	);

	// Instance for pv_avgu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_h pv_avgu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_h_instruction)
		, .operand_rs2(operand_rs2_pv_avgu_h)
		, .operand_rs1(operand_rs1_pv_avgu_h)
		, .operand_rd(operand_rd_pv_avgu_h)
	);

	// Instance for pv_avgu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sc_h pv_avgu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_avgu_sc_h)
		, .operand_rs1(operand_rs1_pv_avgu_sc_h)
		, .operand_rd(operand_rd_pv_avgu_sc_h)
	);

	// Instance for pv_avgu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sci_h pv_avgu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_avgu_sci_h)
		, .operand_rs1(operand_rs1_pv_avgu_sci_h)
		, .operand_rd(operand_rd_pv_avgu_sci_h)
	);

	// Instance for pv_avgu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_b pv_avgu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_b_instruction)
		, .operand_rs2(operand_rs2_pv_avgu_b)
		, .operand_rs1(operand_rs1_pv_avgu_b)
		, .operand_rd(operand_rd_pv_avgu_b)
	);

	// Instance for pv_avgu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sc_b pv_avgu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_avgu_sc_b)
		, .operand_rs1(operand_rs1_pv_avgu_sc_b)
		, .operand_rd(operand_rd_pv_avgu_sc_b)
	);

	// Instance for pv_avgu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_avgu_sci_b pv_avgu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_avgu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_avgu_sci_b)
		, .operand_rs1(operand_rs1_pv_avgu_sci_b)
		, .operand_rd(operand_rd_pv_avgu_sci_b)
	);

	// Instance for pv_min_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_h pv_min_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_h_instruction)
		, .operand_rs2(operand_rs2_pv_min_h)
		, .operand_rs1(operand_rs1_pv_min_h)
		, .operand_rd(operand_rd_pv_min_h)
	);

	// Instance for pv_min_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_sc_h pv_min_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_min_sc_h)
		, .operand_rs1(operand_rs1_pv_min_sc_h)
		, .operand_rd(operand_rd_pv_min_sc_h)
	);

	// Instance for pv_min_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_sci_h pv_min_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_sci_h_instruction)
		, .operand_imm(operand_imm_pv_min_sci_h)
		, .operand_rs1(operand_rs1_pv_min_sci_h)
		, .operand_rd(operand_rd_pv_min_sci_h)
	);

	// Instance for pv_min_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_b pv_min_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_b_instruction)
		, .operand_rs2(operand_rs2_pv_min_b)
		, .operand_rs1(operand_rs1_pv_min_b)
		, .operand_rd(operand_rd_pv_min_b)
	);

	// Instance for pv_min_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_sc_b pv_min_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_min_sc_b)
		, .operand_rs1(operand_rs1_pv_min_sc_b)
		, .operand_rd(operand_rd_pv_min_sc_b)
	);

	// Instance for pv_min_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_min_sci_b pv_min_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_min_sci_b_instruction)
		, .operand_imm(operand_imm_pv_min_sci_b)
		, .operand_rs1(operand_rs1_pv_min_sci_b)
		, .operand_rd(operand_rd_pv_min_sci_b)
	);

	// Instance for pv_minu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_h pv_minu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_h_instruction)
		, .operand_rs2(operand_rs2_pv_minu_h)
		, .operand_rs1(operand_rs1_pv_minu_h)
		, .operand_rd(operand_rd_pv_minu_h)
	);

	// Instance for pv_minu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_sc_h pv_minu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_minu_sc_h)
		, .operand_rs1(operand_rs1_pv_minu_sc_h)
		, .operand_rd(operand_rd_pv_minu_sc_h)
	);

	// Instance for pv_minu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_sci_h pv_minu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_minu_sci_h)
		, .operand_rs1(operand_rs1_pv_minu_sci_h)
		, .operand_rd(operand_rd_pv_minu_sci_h)
	);

	// Instance for pv_minu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_b pv_minu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_b_instruction)
		, .operand_rs2(operand_rs2_pv_minu_b)
		, .operand_rs1(operand_rs1_pv_minu_b)
		, .operand_rd(operand_rd_pv_minu_b)
	);

	// Instance for pv_minu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_sc_b pv_minu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_minu_sc_b)
		, .operand_rs1(operand_rs1_pv_minu_sc_b)
		, .operand_rd(operand_rd_pv_minu_sc_b)
	);

	// Instance for pv_minu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_minu_sci_b pv_minu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_minu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_minu_sci_b)
		, .operand_rs1(operand_rs1_pv_minu_sci_b)
		, .operand_rd(operand_rd_pv_minu_sci_b)
	);

	// Instance for pv_max_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_h pv_max_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_h_instruction)
		, .operand_rs2(operand_rs2_pv_max_h)
		, .operand_rs1(operand_rs1_pv_max_h)
		, .operand_rd(operand_rd_pv_max_h)
	);

	// Instance for pv_max_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_sc_h pv_max_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_max_sc_h)
		, .operand_rs1(operand_rs1_pv_max_sc_h)
		, .operand_rd(operand_rd_pv_max_sc_h)
	);

	// Instance for pv_max_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_sci_h pv_max_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_sci_h_instruction)
		, .operand_imm(operand_imm_pv_max_sci_h)
		, .operand_rs1(operand_rs1_pv_max_sci_h)
		, .operand_rd(operand_rd_pv_max_sci_h)
	);

	// Instance for pv_max_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_b pv_max_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_b_instruction)
		, .operand_rs2(operand_rs2_pv_max_b)
		, .operand_rs1(operand_rs1_pv_max_b)
		, .operand_rd(operand_rd_pv_max_b)
	);

	// Instance for pv_max_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_sc_b pv_max_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_max_sc_b)
		, .operand_rs1(operand_rs1_pv_max_sc_b)
		, .operand_rd(operand_rd_pv_max_sc_b)
	);

	// Instance for pv_max_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_max_sci_b pv_max_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_max_sci_b_instruction)
		, .operand_imm(operand_imm_pv_max_sci_b)
		, .operand_rs1(operand_rs1_pv_max_sci_b)
		, .operand_rd(operand_rd_pv_max_sci_b)
	);

	// Instance for pv_maxu_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_h pv_maxu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_h_instruction)
		, .operand_rs2(operand_rs2_pv_maxu_h)
		, .operand_rs1(operand_rs1_pv_maxu_h)
		, .operand_rd(operand_rd_pv_maxu_h)
	);

	// Instance for pv_maxu_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sc_h pv_maxu_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_maxu_sc_h)
		, .operand_rs1(operand_rs1_pv_maxu_sc_h)
		, .operand_rd(operand_rd_pv_maxu_sc_h)
	);

	// Instance for pv_maxu_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sci_h pv_maxu_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_sci_h_instruction)
		, .operand_imm(operand_imm_pv_maxu_sci_h)
		, .operand_rs1(operand_rs1_pv_maxu_sci_h)
		, .operand_rd(operand_rd_pv_maxu_sci_h)
	);

	// Instance for pv_maxu_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_b pv_maxu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_b_instruction)
		, .operand_rs2(operand_rs2_pv_maxu_b)
		, .operand_rs1(operand_rs1_pv_maxu_b)
		, .operand_rd(operand_rd_pv_maxu_b)
	);

	// Instance for pv_maxu_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sc_b pv_maxu_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_maxu_sc_b)
		, .operand_rs1(operand_rs1_pv_maxu_sc_b)
		, .operand_rd(operand_rd_pv_maxu_sc_b)
	);

	// Instance for pv_maxu_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_maxu_sci_b pv_maxu_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_maxu_sci_b_instruction)
		, .operand_imm(operand_imm_pv_maxu_sci_b)
		, .operand_rs1(operand_rs1_pv_maxu_sci_b)
		, .operand_rd(operand_rd_pv_maxu_sci_b)
	);

	// Instance for pv_srl_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_h pv_srl_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_h_instruction)
		, .operand_rs2(operand_rs2_pv_srl_h)
		, .operand_rs1(operand_rs1_pv_srl_h)
		, .operand_rd(operand_rd_pv_srl_h)
	);

	// Instance for pv_srl_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_sc_h pv_srl_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_srl_sc_h)
		, .operand_rs1(operand_rs1_pv_srl_sc_h)
		, .operand_rd(operand_rd_pv_srl_sc_h)
	);

	// Instance for pv_srl_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_sci_h pv_srl_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_sci_h_instruction)
		, .operand_imm(operand_imm_pv_srl_sci_h)
		, .operand_rs1(operand_rs1_pv_srl_sci_h)
		, .operand_rd(operand_rd_pv_srl_sci_h)
	);

	// Instance for pv_srl_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_b pv_srl_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_b_instruction)
		, .operand_rs2(operand_rs2_pv_srl_b)
		, .operand_rs1(operand_rs1_pv_srl_b)
		, .operand_rd(operand_rd_pv_srl_b)
	);

	// Instance for pv_srl_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_sc_b pv_srl_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_srl_sc_b)
		, .operand_rs1(operand_rs1_pv_srl_sc_b)
		, .operand_rd(operand_rd_pv_srl_sc_b)
	);

	// Instance for pv_srl_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_srl_sci_b pv_srl_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_srl_sci_b_instruction)
		, .operand_imm(operand_imm_pv_srl_sci_b)
		, .operand_rs1(operand_rs1_pv_srl_sci_b)
		, .operand_rd(operand_rd_pv_srl_sci_b)
	);

	// Instance for pv_sra_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_h pv_sra_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_h_instruction)
		, .operand_rs2(operand_rs2_pv_sra_h)
		, .operand_rs1(operand_rs1_pv_sra_h)
		, .operand_rd(operand_rd_pv_sra_h)
	);

	// Instance for pv_sra_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_sc_h pv_sra_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sra_sc_h)
		, .operand_rs1(operand_rs1_pv_sra_sc_h)
		, .operand_rd(operand_rd_pv_sra_sc_h)
	);

	// Instance for pv_sra_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_sci_h pv_sra_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sra_sci_h)
		, .operand_rs1(operand_rs1_pv_sra_sci_h)
		, .operand_rd(operand_rd_pv_sra_sci_h)
	);

	// Instance for pv_sra_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_b pv_sra_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_b_instruction)
		, .operand_rs2(operand_rs2_pv_sra_b)
		, .operand_rs1(operand_rs1_pv_sra_b)
		, .operand_rd(operand_rd_pv_sra_b)
	);

	// Instance for pv_sra_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_sc_b pv_sra_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sra_sc_b)
		, .operand_rs1(operand_rs1_pv_sra_sc_b)
		, .operand_rd(operand_rd_pv_sra_sc_b)
	);

	// Instance for pv_sra_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sra_sci_b pv_sra_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sra_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sra_sci_b)
		, .operand_rs1(operand_rs1_pv_sra_sci_b)
		, .operand_rd(operand_rd_pv_sra_sci_b)
	);

	// Instance for pv_sll_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_h pv_sll_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_h_instruction)
		, .operand_rs2(operand_rs2_pv_sll_h)
		, .operand_rs1(operand_rs1_pv_sll_h)
		, .operand_rd(operand_rd_pv_sll_h)
	);

	// Instance for pv_sll_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_sc_h pv_sll_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sll_sc_h)
		, .operand_rs1(operand_rs1_pv_sll_sc_h)
		, .operand_rd(operand_rd_pv_sll_sc_h)
	);

	// Instance for pv_sll_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_sci_h pv_sll_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sll_sci_h)
		, .operand_rs1(operand_rs1_pv_sll_sci_h)
		, .operand_rd(operand_rd_pv_sll_sci_h)
	);

	// Instance for pv_sll_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_b pv_sll_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_b_instruction)
		, .operand_rs2(operand_rs2_pv_sll_b)
		, .operand_rs1(operand_rs1_pv_sll_b)
		, .operand_rd(operand_rd_pv_sll_b)
	);

	// Instance for pv_sll_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_sc_b pv_sll_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sll_sc_b)
		, .operand_rs1(operand_rs1_pv_sll_sc_b)
		, .operand_rd(operand_rd_pv_sll_sc_b)
	);

	// Instance for pv_sll_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sll_sci_b pv_sll_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sll_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sll_sci_b)
		, .operand_rs1(operand_rs1_pv_sll_sci_b)
		, .operand_rd(operand_rd_pv_sll_sci_b)
	);

	// Instance for pv_or_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_h pv_or_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_h_instruction)
		, .operand_rs2(operand_rs2_pv_or_h)
		, .operand_rs1(operand_rs1_pv_or_h)
		, .operand_rd(operand_rd_pv_or_h)
	);

	// Instance for pv_or_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_sc_h pv_or_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_or_sc_h)
		, .operand_rs1(operand_rs1_pv_or_sc_h)
		, .operand_rd(operand_rd_pv_or_sc_h)
	);

	// Instance for pv_or_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_sci_h pv_or_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_sci_h_instruction)
		, .operand_imm(operand_imm_pv_or_sci_h)
		, .operand_rs1(operand_rs1_pv_or_sci_h)
		, .operand_rd(operand_rd_pv_or_sci_h)
	);

	// Instance for pv_or_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_b pv_or_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_b_instruction)
		, .operand_rs2(operand_rs2_pv_or_b)
		, .operand_rs1(operand_rs1_pv_or_b)
		, .operand_rd(operand_rd_pv_or_b)
	);

	// Instance for pv_or_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_sc_b pv_or_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_or_sc_b)
		, .operand_rs1(operand_rs1_pv_or_sc_b)
		, .operand_rd(operand_rd_pv_or_sc_b)
	);

	// Instance for pv_or_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_or_sci_b pv_or_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_or_sci_b_instruction)
		, .operand_imm(operand_imm_pv_or_sci_b)
		, .operand_rs1(operand_rs1_pv_or_sci_b)
		, .operand_rd(operand_rd_pv_or_sci_b)
	);

	// Instance for pv_xor_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_h pv_xor_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_h_instruction)
		, .operand_rs2(operand_rs2_pv_xor_h)
		, .operand_rs1(operand_rs1_pv_xor_h)
		, .operand_rd(operand_rd_pv_xor_h)
	);

	// Instance for pv_xor_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_sc_h pv_xor_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_xor_sc_h)
		, .operand_rs1(operand_rs1_pv_xor_sc_h)
		, .operand_rd(operand_rd_pv_xor_sc_h)
	);

	// Instance for pv_xor_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_sci_h pv_xor_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_sci_h_instruction)
		, .operand_imm(operand_imm_pv_xor_sci_h)
		, .operand_rs1(operand_rs1_pv_xor_sci_h)
		, .operand_rd(operand_rd_pv_xor_sci_h)
	);

	// Instance for pv_xor_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_b pv_xor_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_b_instruction)
		, .operand_rs2(operand_rs2_pv_xor_b)
		, .operand_rs1(operand_rs1_pv_xor_b)
		, .operand_rd(operand_rd_pv_xor_b)
	);

	// Instance for pv_xor_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_sc_b pv_xor_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_xor_sc_b)
		, .operand_rs1(operand_rs1_pv_xor_sc_b)
		, .operand_rd(operand_rd_pv_xor_sc_b)
	);

	// Instance for pv_xor_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_xor_sci_b pv_xor_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_xor_sci_b_instruction)
		, .operand_imm(operand_imm_pv_xor_sci_b)
		, .operand_rs1(operand_rs1_pv_xor_sci_b)
		, .operand_rd(operand_rd_pv_xor_sci_b)
	);

	// Instance for pv_and_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_h pv_and_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_h_instruction)
		, .operand_rs2(operand_rs2_pv_and_h)
		, .operand_rs1(operand_rs1_pv_and_h)
		, .operand_rd(operand_rd_pv_and_h)
	);

	// Instance for pv_and_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_sc_h pv_and_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_and_sc_h)
		, .operand_rs1(operand_rs1_pv_and_sc_h)
		, .operand_rd(operand_rd_pv_and_sc_h)
	);

	// Instance for pv_and_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_sci_h pv_and_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_sci_h_instruction)
		, .operand_imm(operand_imm_pv_and_sci_h)
		, .operand_rs1(operand_rs1_pv_and_sci_h)
		, .operand_rd(operand_rd_pv_and_sci_h)
	);

	// Instance for pv_and_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_b pv_and_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_b_instruction)
		, .operand_rs2(operand_rs2_pv_and_b)
		, .operand_rs1(operand_rs1_pv_and_b)
		, .operand_rd(operand_rd_pv_and_b)
	);

	// Instance for pv_and_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_sc_b pv_and_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_and_sc_b)
		, .operand_rs1(operand_rs1_pv_and_sc_b)
		, .operand_rd(operand_rd_pv_and_sc_b)
	);

	// Instance for pv_and_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_and_sci_b pv_and_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_and_sci_b_instruction)
		, .operand_imm(operand_imm_pv_and_sci_b)
		, .operand_rs1(operand_rs1_pv_and_sci_b)
		, .operand_rd(operand_rd_pv_and_sci_b)
	);

	// Instance for pv_abs_h instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_abs_h pv_abs_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_abs_h_instruction)
		, .operand_rs1(operand_rs1_pv_abs_h)
		, .operand_rd(operand_rd_pv_abs_h)
	);

	// Instance for pv_abs_b instruction
	// Operands [rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_abs_b pv_abs_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_abs_b_instruction)
		, .operand_rs1(operand_rs1_pv_abs_b)
		, .operand_rd(operand_rd_pv_abs_b)
	);

	// Instance for pv_extract_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_extract_h pv_extract_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_extract_h_instruction)
		, .operand_imm(operand_imm_pv_extract_h)
		, .operand_rs1(operand_rs1_pv_extract_h)
		, .operand_rd(operand_rd_pv_extract_h)
	);

	// Instance for pv_extract_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_extract_b pv_extract_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_extract_b_instruction)
		, .operand_imm(operand_imm_pv_extract_b)
		, .operand_rs1(operand_rs1_pv_extract_b)
		, .operand_rd(operand_rd_pv_extract_b)
	);

	// Instance for pv_extractu_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_extractu_h pv_extractu_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_extractu_h_instruction)
		, .operand_imm(operand_imm_pv_extractu_h)
		, .operand_rs1(operand_rs1_pv_extractu_h)
		, .operand_rd(operand_rd_pv_extractu_h)
	);

	// Instance for pv_extractu_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_extractu_b pv_extractu_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_extractu_b_instruction)
		, .operand_imm(operand_imm_pv_extractu_b)
		, .operand_rs1(operand_rs1_pv_extractu_b)
		, .operand_rd(operand_rd_pv_extractu_b)
	);

	// Instance for pv_insert_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_insert_h pv_insert_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_insert_h_instruction)
		, .operand_imm(operand_imm_pv_insert_h)
		, .operand_rs1(operand_rs1_pv_insert_h)
		, .operand_rd(operand_rd_pv_insert_h)
	);

	// Instance for pv_insert_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_insert_b pv_insert_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_insert_b_instruction)
		, .operand_imm(operand_imm_pv_insert_b)
		, .operand_rs1(operand_rs1_pv_insert_b)
		, .operand_rd(operand_rd_pv_insert_b)
	);

	// Instance for pv_dotup_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_h pv_dotup_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotup_h)
		, .operand_rs1(operand_rs1_pv_dotup_h)
		, .operand_rd(operand_rd_pv_dotup_h)
	);

	// Instance for pv_dotup_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sc_h pv_dotup_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotup_sc_h)
		, .operand_rs1(operand_rs1_pv_dotup_sc_h)
		, .operand_rd(operand_rd_pv_dotup_sc_h)
	);

	// Instance for pv_dotup_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sci_h pv_dotup_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_sci_h_instruction)
		, .operand_imm(operand_imm_pv_dotup_sci_h)
		, .operand_rs1(operand_rs1_pv_dotup_sci_h)
		, .operand_rd(operand_rd_pv_dotup_sci_h)
	);

	// Instance for pv_dotup_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_b pv_dotup_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotup_b)
		, .operand_rs1(operand_rs1_pv_dotup_b)
		, .operand_rd(operand_rd_pv_dotup_b)
	);

	// Instance for pv_dotup_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sc_b pv_dotup_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotup_sc_b)
		, .operand_rs1(operand_rs1_pv_dotup_sc_b)
		, .operand_rd(operand_rd_pv_dotup_sc_b)
	);

	// Instance for pv_dotup_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotup_sci_b pv_dotup_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotup_sci_b_instruction)
		, .operand_imm(operand_imm_pv_dotup_sci_b)
		, .operand_rs1(operand_rs1_pv_dotup_sci_b)
		, .operand_rd(operand_rd_pv_dotup_sci_b)
	);

	// Instance for pv_dotusp_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_h pv_dotusp_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotusp_h)
		, .operand_rs1(operand_rs1_pv_dotusp_h)
		, .operand_rd(operand_rd_pv_dotusp_h)
	);

	// Instance for pv_dotusp_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sc_h pv_dotusp_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotusp_sc_h)
		, .operand_rs1(operand_rs1_pv_dotusp_sc_h)
		, .operand_rd(operand_rd_pv_dotusp_sc_h)
	);

	// Instance for pv_dotusp_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sci_h pv_dotusp_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_sci_h_instruction)
		, .operand_imm(operand_imm_pv_dotusp_sci_h)
		, .operand_rs1(operand_rs1_pv_dotusp_sci_h)
		, .operand_rd(operand_rd_pv_dotusp_sci_h)
	);

	// Instance for pv_dotusp_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_b pv_dotusp_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotusp_b)
		, .operand_rs1(operand_rs1_pv_dotusp_b)
		, .operand_rd(operand_rd_pv_dotusp_b)
	);

	// Instance for pv_dotusp_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sc_b pv_dotusp_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotusp_sc_b)
		, .operand_rs1(operand_rs1_pv_dotusp_sc_b)
		, .operand_rd(operand_rd_pv_dotusp_sc_b)
	);

	// Instance for pv_dotusp_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotusp_sci_b pv_dotusp_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotusp_sci_b_instruction)
		, .operand_imm(operand_imm_pv_dotusp_sci_b)
		, .operand_rs1(operand_rs1_pv_dotusp_sci_b)
		, .operand_rd(operand_rd_pv_dotusp_sci_b)
	);

	// Instance for pv_dotsp_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_h pv_dotsp_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotsp_h)
		, .operand_rs1(operand_rs1_pv_dotsp_h)
		, .operand_rd(operand_rd_pv_dotsp_h)
	);

	// Instance for pv_dotsp_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sc_h pv_dotsp_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_dotsp_sc_h)
		, .operand_rs1(operand_rs1_pv_dotsp_sc_h)
		, .operand_rd(operand_rd_pv_dotsp_sc_h)
	);

	// Instance for pv_dotsp_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sci_h pv_dotsp_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_sci_h_instruction)
		, .operand_imm(operand_imm_pv_dotsp_sci_h)
		, .operand_rs1(operand_rs1_pv_dotsp_sci_h)
		, .operand_rd(operand_rd_pv_dotsp_sci_h)
	);

	// Instance for pv_dotsp_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_b pv_dotsp_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotsp_b)
		, .operand_rs1(operand_rs1_pv_dotsp_b)
		, .operand_rd(operand_rd_pv_dotsp_b)
	);

	// Instance for pv_dotsp_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sc_b pv_dotsp_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_dotsp_sc_b)
		, .operand_rs1(operand_rs1_pv_dotsp_sc_b)
		, .operand_rd(operand_rd_pv_dotsp_sc_b)
	);

	// Instance for pv_dotsp_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_dotsp_sci_b pv_dotsp_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_dotsp_sci_b_instruction)
		, .operand_imm(operand_imm_pv_dotsp_sci_b)
		, .operand_rs1(operand_rs1_pv_dotsp_sci_b)
		, .operand_rd(operand_rd_pv_dotsp_sci_b)
	);

	// Instance for pv_sdotup_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_h pv_sdotup_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotup_h)
		, .operand_rs1(operand_rs1_pv_sdotup_h)
		, .operand_rd(operand_rd_pv_sdotup_h)
	);

	// Instance for pv_sdotup_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sc_h pv_sdotup_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotup_sc_h)
		, .operand_rs1(operand_rs1_pv_sdotup_sc_h)
		, .operand_rd(operand_rd_pv_sdotup_sc_h)
	);

	// Instance for pv_sdotup_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sci_h pv_sdotup_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sdotup_sci_h)
		, .operand_rs1(operand_rs1_pv_sdotup_sci_h)
		, .operand_rd(operand_rd_pv_sdotup_sci_h)
	);

	// Instance for pv_sdotup_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_b pv_sdotup_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotup_b)
		, .operand_rs1(operand_rs1_pv_sdotup_b)
		, .operand_rd(operand_rd_pv_sdotup_b)
	);

	// Instance for pv_sdotup_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sc_b pv_sdotup_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotup_sc_b)
		, .operand_rs1(operand_rs1_pv_sdotup_sc_b)
		, .operand_rd(operand_rd_pv_sdotup_sc_b)
	);

	// Instance for pv_sdotup_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotup_sci_b pv_sdotup_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotup_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sdotup_sci_b)
		, .operand_rs1(operand_rs1_pv_sdotup_sci_b)
		, .operand_rd(operand_rd_pv_sdotup_sci_b)
	);

	// Instance for pv_sdotusp_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_h pv_sdotusp_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotusp_h)
		, .operand_rs1(operand_rs1_pv_sdotusp_h)
		, .operand_rd(operand_rd_pv_sdotusp_h)
	);

	// Instance for pv_sdotusp_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sc_h pv_sdotusp_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotusp_sc_h)
		, .operand_rs1(operand_rs1_pv_sdotusp_sc_h)
		, .operand_rd(operand_rd_pv_sdotusp_sc_h)
	);

	// Instance for pv_sdotusp_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sci_h pv_sdotusp_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sdotusp_sci_h)
		, .operand_rs1(operand_rs1_pv_sdotusp_sci_h)
		, .operand_rd(operand_rd_pv_sdotusp_sci_h)
	);

	// Instance for pv_sdotusp_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_b pv_sdotusp_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotusp_b)
		, .operand_rs1(operand_rs1_pv_sdotusp_b)
		, .operand_rd(operand_rd_pv_sdotusp_b)
	);

	// Instance for pv_sdotusp_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sc_b pv_sdotusp_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotusp_sc_b)
		, .operand_rs1(operand_rs1_pv_sdotusp_sc_b)
		, .operand_rd(operand_rd_pv_sdotusp_sc_b)
	);

	// Instance for pv_sdotusp_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotusp_sci_b pv_sdotusp_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotusp_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sdotusp_sci_b)
		, .operand_rs1(operand_rs1_pv_sdotusp_sci_b)
		, .operand_rd(operand_rd_pv_sdotusp_sci_b)
	);

	// Instance for pv_sdotsp_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_h pv_sdotsp_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotsp_h)
		, .operand_rs1(operand_rs1_pv_sdotsp_h)
		, .operand_rd(operand_rd_pv_sdotsp_h)
	);

	// Instance for pv_sdotsp_sc_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sc_h pv_sdotsp_sc_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_sc_h_instruction)
		, .operand_rs2(operand_rs2_pv_sdotsp_sc_h)
		, .operand_rs1(operand_rs1_pv_sdotsp_sc_h)
		, .operand_rd(operand_rd_pv_sdotsp_sc_h)
	);

	// Instance for pv_sdotsp_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sci_h pv_sdotsp_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_sci_h_instruction)
		, .operand_imm(operand_imm_pv_sdotsp_sci_h)
		, .operand_rs1(operand_rs1_pv_sdotsp_sci_h)
		, .operand_rd(operand_rd_pv_sdotsp_sci_h)
	);

	// Instance for pv_sdotsp_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_b pv_sdotsp_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotsp_b)
		, .operand_rs1(operand_rs1_pv_sdotsp_b)
		, .operand_rd(operand_rd_pv_sdotsp_b)
	);

	// Instance for pv_sdotsp_sc_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sc_b pv_sdotsp_sc_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_sc_b_instruction)
		, .operand_rs2(operand_rs2_pv_sdotsp_sc_b)
		, .operand_rs1(operand_rs1_pv_sdotsp_sc_b)
		, .operand_rd(operand_rd_pv_sdotsp_sc_b)
	);

	// Instance for pv_sdotsp_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_sdotsp_sci_b pv_sdotsp_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_sdotsp_sci_b_instruction)
		, .operand_imm(operand_imm_pv_sdotsp_sci_b)
		, .operand_rs1(operand_rs1_pv_sdotsp_sci_b)
		, .operand_rd(operand_rd_pv_sdotsp_sci_b)
	);

	// Instance for pv_shuffle_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_h pv_shuffle_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shuffle_h_instruction)
		, .operand_rs2(operand_rs2_pv_shuffle_h)
		, .operand_rs1(operand_rs1_pv_shuffle_h)
		, .operand_rd(operand_rd_pv_shuffle_h)
	);

	// Instance for pv_shuffle_sci_h instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_sci_h pv_shuffle_sci_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shuffle_sci_h_instruction)
		, .operand_imm(operand_imm_pv_shuffle_sci_h)
		, .operand_rs1(operand_rs1_pv_shuffle_sci_h)
		, .operand_rd(operand_rd_pv_shuffle_sci_h)
	);

	// Instance for pv_shuffle_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shuffle_b pv_shuffle_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shuffle_b_instruction)
		, .operand_rs2(operand_rs2_pv_shuffle_b)
		, .operand_rs1(operand_rs1_pv_shuffle_b)
		, .operand_rd(operand_rd_pv_shuffle_b)
	);

	// Instance for pv_shufflei0_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shufflei0_sci_b pv_shufflei0_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shufflei0_sci_b_instruction)
		, .operand_imm(operand_imm_pv_shufflei0_sci_b)
		, .operand_rs1(operand_rs1_pv_shufflei0_sci_b)
		, .operand_rd(operand_rd_pv_shufflei0_sci_b)
	);

	// Instance for pv_shufflei1_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shufflei1_sci_b pv_shufflei1_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shufflei1_sci_b_instruction)
		, .operand_imm(operand_imm_pv_shufflei1_sci_b)
		, .operand_rs1(operand_rs1_pv_shufflei1_sci_b)
		, .operand_rd(operand_rd_pv_shufflei1_sci_b)
	);

	// Instance for pv_shufflei2_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shufflei2_sci_b pv_shufflei2_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shufflei2_sci_b_instruction)
		, .operand_imm(operand_imm_pv_shufflei2_sci_b)
		, .operand_rs1(operand_rs1_pv_shufflei2_sci_b)
		, .operand_rd(operand_rd_pv_shufflei2_sci_b)
	);

	// Instance for pv_shufflei3_sci_b instruction
	// Operands [imm (6 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shufflei3_sci_b pv_shufflei3_sci_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shufflei3_sci_b_instruction)
		, .operand_imm(operand_imm_pv_shufflei3_sci_b)
		, .operand_rs1(operand_rs1_pv_shufflei3_sci_b)
		, .operand_rd(operand_rd_pv_shufflei3_sci_b)
	);

	// Instance for pv_shuffle2_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shuffle2_h pv_shuffle2_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shuffle2_h_instruction)
		, .operand_rs2(operand_rs2_pv_shuffle2_h)
		, .operand_rs1(operand_rs1_pv_shuffle2_h)
		, .operand_rd(operand_rd_pv_shuffle2_h)
	);

	// Instance for pv_shuffle2_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_shuffle2_b pv_shuffle2_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_shuffle2_b_instruction)
		, .operand_rs2(operand_rs2_pv_shuffle2_b)
		, .operand_rs1(operand_rs1_pv_shuffle2_b)
		, .operand_rd(operand_rd_pv_shuffle2_b)
	);

	// Instance for pv_pack_h instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_pack_h pv_pack_h_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_pack_h_instruction)
		, .operand_rs2(operand_rs2_pv_pack_h)
		, .operand_rs1(operand_rs1_pv_pack_h)
		, .operand_rd(operand_rd_pv_pack_h)
	);

	// Instance for pv_packhi_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_packhi_b pv_packhi_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_packhi_b_instruction)
		, .operand_rs2(operand_rs2_pv_packhi_b)
		, .operand_rs1(operand_rs1_pv_packhi_b)
		, .operand_rd(operand_rd_pv_packhi_b)
	);

	// Instance for pv_packlo_b instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32im_zicsr_xpulp2_ri5cy_pv_packlo_b pv_packlo_b_inst (
		  .opcode(opcode)
		, .opcode_valid(is_pv_packlo_b_instruction)
		, .operand_rs2(operand_rs2_pv_packlo_b)
		, .operand_rs1(operand_rs1_pv_packlo_b)
		, .operand_rd(operand_rd_pv_packlo_b)
	);


	assign opcode_valid = 0
		|| is_lui_instruction
		|| is_auipc_instruction
		|| is_jal_instruction
		|| is_jalr_instruction
		|| is_beq_instruction
		|| is_bne_instruction
		|| is_blt_instruction
		|| is_bge_instruction
		|| is_bltu_instruction
		|| is_bgeu_instruction
		|| is_lb_instruction
		|| is_lh_instruction
		|| is_lw_instruction
		|| is_lbu_instruction
		|| is_lhu_instruction
		|| is_sb_instruction
		|| is_sh_instruction
		|| is_sw_instruction
		|| is_addi_instruction
		|| is_slti_instruction
		|| is_sltiu_instruction
		|| is_xori_instruction
		|| is_ori_instruction
		|| is_andi_instruction
		|| is_slli_instruction
		|| is_srli_instruction
		|| is_srai_instruction
		|| is_add_instruction
		|| is_sub_instruction
		|| is_sll_instruction
		|| is_slt_instruction
		|| is_sltu_instruction
		|| is_xor_instruction
		|| is_srl_instruction
		|| is_sra_instruction
		|| is_or_instruction
		|| is_and_instruction
		|| is_fence_instruction
		|| is_ecall_instruction
		|| is_ebreak_instruction
		|| is_mul_instruction
		|| is_mulh_instruction
		|| is_mulhsu_instruction
		|| is_mulhu_instruction
		|| is_div_instruction
		|| is_divu_instruction
		|| is_rem_instruction
		|| is_remu_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction
		|| is_p_extract_instruction
		|| is_p_extractu_instruction
		|| is_p_insert_instruction
		|| is_p_bclr_instruction
		|| is_p_bset_instruction
		|| is_p_extractr_instruction
		|| is_p_extractur_instruction
		|| is_p_insertr_instruction
		|| is_p_bclrr_instruction
		|| is_p_bsetr_instruction
		|| is_p_ror_instruction
		|| is_p_ff1_instruction
		|| is_p_fl1_instruction
		|| is_p_clb_instruction
		|| is_p_cnt_instruction
		|| is_p_abs_instruction
		|| is_p_slet_instruction
		|| is_p_sletu_instruction
		|| is_p_min_instruction
		|| is_p_minu_instruction
		|| is_p_max_instruction
		|| is_p_maxu_instruction
		|| is_p_exths_instruction
		|| is_p_exthz_instruction
		|| is_p_extbs_instruction
		|| is_p_extbz_instruction
		|| is_p_clip_instruction
		|| is_p_clipu_instruction
		|| is_p_clipr_instruction
		|| is_p_clipur_instruction
		|| is_p_addn_instruction
		|| is_p_addun_instruction
		|| is_p_addrn_instruction
		|| is_p_addurn_instruction
		|| is_p_subn_instruction
		|| is_p_subun_instruction
		|| is_p_subrn_instruction
		|| is_p_suburn_instruction
		|| is_p_addnr_instruction
		|| is_p_addunr_instruction
		|| is_p_addrnr_instruction
		|| is_p_addurnr_instruction
		|| is_p_subnr_instruction
		|| is_p_subunr_instruction
		|| is_p_subrnr_instruction
		|| is_p_suburnr_instruction
		|| is_lp_starti_instruction
		|| is_lp_endi_instruction
		|| is_lp_counti_instruction
		|| is_lp_count_instruction
		|| is_lp_setup_instruction
		|| is_lp_setupi_instruction
		|| is_p_beqimm_instruction
		|| is_p_bneimm_instruction
		|| is_p_lb_i_inc_instruction
		|| is_p_lbu_i_inc_instruction
		|| is_p_lh_i_inc_instruction
		|| is_p_lhu_i_inc_instruction
		|| is_p_lw_i_inc_instruction
		|| is_p_lb_r_inc_instruction
		|| is_p_lbu_r_inc_instruction
		|| is_p_lh_r_inc_instruction
		|| is_p_lhu_r_inc_instruction
		|| is_p_lw_r_inc_instruction
		|| is_p_lb_r_instruction
		|| is_p_lbu_r_instruction
		|| is_p_lh_r_instruction
		|| is_p_lhu_r_instruction
		|| is_p_lw_r_instruction
		|| is_p_mulu_instruction
		|| is_p_mulhhu_instruction
		|| is_p_mulun_instruction
		|| is_p_mulhhun_instruction
		|| is_p_mulurn_instruction
		|| is_p_mulhhurn_instruction
		|| is_p_macsn_instruction
		|| is_p_machhsn_instruction
		|| is_p_macsrn_instruction
		|| is_p_machhsrn_instruction
		|| is_p_macun_instruction
		|| is_p_machhun_instruction
		|| is_p_macurn_instruction
		|| is_p_machhurn_instruction
		|| is_p_sb_i_inc_instruction
		|| is_p_sh_i_inc_instruction
		|| is_p_sw_i_inc_instruction
		|| is_p_sb_r_inc_instruction
		|| is_p_sh_r_inc_instruction
		|| is_p_sw_r_inc_instruction
		|| is_p_sb_r_instruction
		|| is_p_sh_r_instruction
		|| is_p_sw_r_instruction
		|| is_pv_cmpeq_h_instruction
		|| is_pv_cmpeq_sc_h_instruction
		|| is_pv_cmpeq_sci_h_instruction
		|| is_pv_cmpeq_b_instruction
		|| is_pv_cmpeq_sc_b_instruction
		|| is_pv_cmpeq_sci_b_instruction
		|| is_pv_cmpne_h_instruction
		|| is_pv_cmpne_sc_h_instruction
		|| is_pv_cmpne_sci_h_instruction
		|| is_pv_cmpne_b_instruction
		|| is_pv_cmpne_sc_b_instruction
		|| is_pv_cmpne_sci_b_instruction
		|| is_pv_cmpgt_h_instruction
		|| is_pv_cmpgt_sc_h_instruction
		|| is_pv_cmpgt_sci_h_instruction
		|| is_pv_cmpgt_b_instruction
		|| is_pv_cmpgt_sc_b_instruction
		|| is_pv_cmpgt_sci_b_instruction
		|| is_pv_cmpge_h_instruction
		|| is_pv_cmpge_sc_h_instruction
		|| is_pv_cmpge_sci_h_instruction
		|| is_pv_cmpge_b_instruction
		|| is_pv_cmpge_sc_b_instruction
		|| is_pv_cmpge_sci_b_instruction
		|| is_pv_cmplt_h_instruction
		|| is_pv_cmplt_sc_h_instruction
		|| is_pv_cmplt_sci_h_instruction
		|| is_pv_cmplt_b_instruction
		|| is_pv_cmplt_sc_b_instruction
		|| is_pv_cmplt_sci_b_instruction
		|| is_pv_cmple_h_instruction
		|| is_pv_cmple_sc_h_instruction
		|| is_pv_cmple_sci_h_instruction
		|| is_pv_cmple_b_instruction
		|| is_pv_cmple_sc_b_instruction
		|| is_pv_cmple_sci_b_instruction
		|| is_pv_cmpgtu_h_instruction
		|| is_pv_cmpgtu_sc_h_instruction
		|| is_pv_cmpgtu_sci_h_instruction
		|| is_pv_cmpgtu_b_instruction
		|| is_pv_cmpgtu_sc_b_instruction
		|| is_pv_cmpgtu_sci_b_instruction
		|| is_pv_cmpgeu_h_instruction
		|| is_pv_cmpgeu_sc_h_instruction
		|| is_pv_cmpgeu_sci_h_instruction
		|| is_pv_cmpgeu_b_instruction
		|| is_pv_cmpgeu_sc_b_instruction
		|| is_pv_cmpgeu_sci_b_instruction
		|| is_pv_cmpltu_h_instruction
		|| is_pv_cmpltu_sc_h_instruction
		|| is_pv_cmpltu_sci_h_instruction
		|| is_pv_cmpltu_b_instruction
		|| is_pv_cmpltu_sc_b_instruction
		|| is_pv_cmpltu_sci_b_instruction
		|| is_pv_cmpleu_h_instruction
		|| is_pv_cmpleu_sc_h_instruction
		|| is_pv_cmpleu_sci_h_instruction
		|| is_pv_cmpleu_b_instruction
		|| is_pv_cmpleu_sc_b_instruction
		|| is_pv_cmpleu_sci_b_instruction
		|| is_pv_add_h_instruction
		|| is_pv_add_sc_h_instruction
		|| is_pv_add_sci_h_instruction
		|| is_pv_add_b_instruction
		|| is_pv_add_sc_b_instruction
		|| is_pv_add_sci_b_instruction
		|| is_pv_sub_h_instruction
		|| is_pv_sub_sc_h_instruction
		|| is_pv_sub_sci_h_instruction
		|| is_pv_sub_b_instruction
		|| is_pv_sub_sc_b_instruction
		|| is_pv_sub_sci_b_instruction
		|| is_pv_avg_h_instruction
		|| is_pv_avg_sc_h_instruction
		|| is_pv_avg_sci_h_instruction
		|| is_pv_avg_b_instruction
		|| is_pv_avg_sc_b_instruction
		|| is_pv_avg_sci_b_instruction
		|| is_pv_avgu_h_instruction
		|| is_pv_avgu_sc_h_instruction
		|| is_pv_avgu_sci_h_instruction
		|| is_pv_avgu_b_instruction
		|| is_pv_avgu_sc_b_instruction
		|| is_pv_avgu_sci_b_instruction
		|| is_pv_min_h_instruction
		|| is_pv_min_sc_h_instruction
		|| is_pv_min_sci_h_instruction
		|| is_pv_min_b_instruction
		|| is_pv_min_sc_b_instruction
		|| is_pv_min_sci_b_instruction
		|| is_pv_minu_h_instruction
		|| is_pv_minu_sc_h_instruction
		|| is_pv_minu_sci_h_instruction
		|| is_pv_minu_b_instruction
		|| is_pv_minu_sc_b_instruction
		|| is_pv_minu_sci_b_instruction
		|| is_pv_max_h_instruction
		|| is_pv_max_sc_h_instruction
		|| is_pv_max_sci_h_instruction
		|| is_pv_max_b_instruction
		|| is_pv_max_sc_b_instruction
		|| is_pv_max_sci_b_instruction
		|| is_pv_maxu_h_instruction
		|| is_pv_maxu_sc_h_instruction
		|| is_pv_maxu_sci_h_instruction
		|| is_pv_maxu_b_instruction
		|| is_pv_maxu_sc_b_instruction
		|| is_pv_maxu_sci_b_instruction
		|| is_pv_srl_h_instruction
		|| is_pv_srl_sc_h_instruction
		|| is_pv_srl_sci_h_instruction
		|| is_pv_srl_b_instruction
		|| is_pv_srl_sc_b_instruction
		|| is_pv_srl_sci_b_instruction
		|| is_pv_sra_h_instruction
		|| is_pv_sra_sc_h_instruction
		|| is_pv_sra_sci_h_instruction
		|| is_pv_sra_b_instruction
		|| is_pv_sra_sc_b_instruction
		|| is_pv_sra_sci_b_instruction
		|| is_pv_sll_h_instruction
		|| is_pv_sll_sc_h_instruction
		|| is_pv_sll_sci_h_instruction
		|| is_pv_sll_b_instruction
		|| is_pv_sll_sc_b_instruction
		|| is_pv_sll_sci_b_instruction
		|| is_pv_or_h_instruction
		|| is_pv_or_sc_h_instruction
		|| is_pv_or_sci_h_instruction
		|| is_pv_or_b_instruction
		|| is_pv_or_sc_b_instruction
		|| is_pv_or_sci_b_instruction
		|| is_pv_xor_h_instruction
		|| is_pv_xor_sc_h_instruction
		|| is_pv_xor_sci_h_instruction
		|| is_pv_xor_b_instruction
		|| is_pv_xor_sc_b_instruction
		|| is_pv_xor_sci_b_instruction
		|| is_pv_and_h_instruction
		|| is_pv_and_sc_h_instruction
		|| is_pv_and_sci_h_instruction
		|| is_pv_and_b_instruction
		|| is_pv_and_sc_b_instruction
		|| is_pv_and_sci_b_instruction
		|| is_pv_abs_h_instruction
		|| is_pv_abs_b_instruction
		|| is_pv_extract_h_instruction
		|| is_pv_extract_b_instruction
		|| is_pv_extractu_h_instruction
		|| is_pv_extractu_b_instruction
		|| is_pv_insert_h_instruction
		|| is_pv_insert_b_instruction
		|| is_pv_dotup_h_instruction
		|| is_pv_dotup_sc_h_instruction
		|| is_pv_dotup_sci_h_instruction
		|| is_pv_dotup_b_instruction
		|| is_pv_dotup_sc_b_instruction
		|| is_pv_dotup_sci_b_instruction
		|| is_pv_dotusp_h_instruction
		|| is_pv_dotusp_sc_h_instruction
		|| is_pv_dotusp_sci_h_instruction
		|| is_pv_dotusp_b_instruction
		|| is_pv_dotusp_sc_b_instruction
		|| is_pv_dotusp_sci_b_instruction
		|| is_pv_dotsp_h_instruction
		|| is_pv_dotsp_sc_h_instruction
		|| is_pv_dotsp_sci_h_instruction
		|| is_pv_dotsp_b_instruction
		|| is_pv_dotsp_sc_b_instruction
		|| is_pv_dotsp_sci_b_instruction
		|| is_pv_sdotup_h_instruction
		|| is_pv_sdotup_sc_h_instruction
		|| is_pv_sdotup_sci_h_instruction
		|| is_pv_sdotup_b_instruction
		|| is_pv_sdotup_sc_b_instruction
		|| is_pv_sdotup_sci_b_instruction
		|| is_pv_sdotusp_h_instruction
		|| is_pv_sdotusp_sc_h_instruction
		|| is_pv_sdotusp_sci_h_instruction
		|| is_pv_sdotusp_b_instruction
		|| is_pv_sdotusp_sc_b_instruction
		|| is_pv_sdotusp_sci_b_instruction
		|| is_pv_sdotsp_h_instruction
		|| is_pv_sdotsp_sc_h_instruction
		|| is_pv_sdotsp_sci_h_instruction
		|| is_pv_sdotsp_b_instruction
		|| is_pv_sdotsp_sc_b_instruction
		|| is_pv_sdotsp_sci_b_instruction
		|| is_pv_shuffle_h_instruction
		|| is_pv_shuffle_sci_h_instruction
		|| is_pv_shuffle_b_instruction
		|| is_pv_shufflei0_sci_b_instruction
		|| is_pv_shufflei1_sci_b_instruction
		|| is_pv_shufflei2_sci_b_instruction
		|| is_pv_shufflei3_sci_b_instruction
		|| is_pv_shuffle2_h_instruction
		|| is_pv_shuffle2_b_instruction
		|| is_pv_pack_h_instruction
		|| is_pv_packhi_b_instruction
		|| is_pv_packlo_b_instruction;
	assign has_operand_L = 0
		|| is_lp_starti_instruction
		|| is_lp_endi_instruction
		|| is_lp_counti_instruction
		|| is_lp_count_instruction
		|| is_lp_setup_instruction;
	assign has_operand_csr = 0
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction;
	assign has_operand_fm = 0
		|| is_fence_instruction;
	assign has_operand_imm = 0
		|| is_lui_instruction
		|| is_auipc_instruction
		|| is_jal_instruction
		|| is_jalr_instruction
		|| is_beq_instruction
		|| is_bne_instruction
		|| is_blt_instruction
		|| is_bge_instruction
		|| is_bltu_instruction
		|| is_bgeu_instruction
		|| is_lb_instruction
		|| is_lh_instruction
		|| is_lw_instruction
		|| is_lbu_instruction
		|| is_lhu_instruction
		|| is_sb_instruction
		|| is_sh_instruction
		|| is_sw_instruction
		|| is_addi_instruction
		|| is_slti_instruction
		|| is_sltiu_instruction
		|| is_xori_instruction
		|| is_ori_instruction
		|| is_andi_instruction
		|| is_p_lb_i_inc_instruction
		|| is_p_lbu_i_inc_instruction
		|| is_p_lh_i_inc_instruction
		|| is_p_lhu_i_inc_instruction
		|| is_p_lw_i_inc_instruction
		|| is_p_sb_i_inc_instruction
		|| is_p_sh_i_inc_instruction
		|| is_p_sw_i_inc_instruction
		|| is_pv_cmpeq_sci_h_instruction
		|| is_pv_cmpeq_sci_b_instruction
		|| is_pv_cmpne_sci_h_instruction
		|| is_pv_cmpne_sci_b_instruction
		|| is_pv_cmpgt_sci_h_instruction
		|| is_pv_cmpgt_sci_b_instruction
		|| is_pv_cmpge_sci_h_instruction
		|| is_pv_cmpge_sci_b_instruction
		|| is_pv_cmplt_sci_h_instruction
		|| is_pv_cmplt_sci_b_instruction
		|| is_pv_cmple_sci_h_instruction
		|| is_pv_cmple_sci_b_instruction
		|| is_pv_cmpgtu_sci_h_instruction
		|| is_pv_cmpgtu_sci_b_instruction
		|| is_pv_cmpgeu_sci_h_instruction
		|| is_pv_cmpgeu_sci_b_instruction
		|| is_pv_cmpltu_sci_h_instruction
		|| is_pv_cmpltu_sci_b_instruction
		|| is_pv_cmpleu_sci_h_instruction
		|| is_pv_cmpleu_sci_b_instruction
		|| is_pv_add_sci_h_instruction
		|| is_pv_add_sci_b_instruction
		|| is_pv_sub_sci_h_instruction
		|| is_pv_sub_sci_b_instruction
		|| is_pv_avg_sci_h_instruction
		|| is_pv_avg_sci_b_instruction
		|| is_pv_avgu_sci_h_instruction
		|| is_pv_avgu_sci_b_instruction
		|| is_pv_min_sci_h_instruction
		|| is_pv_min_sci_b_instruction
		|| is_pv_minu_sci_h_instruction
		|| is_pv_minu_sci_b_instruction
		|| is_pv_max_sci_h_instruction
		|| is_pv_max_sci_b_instruction
		|| is_pv_maxu_sci_h_instruction
		|| is_pv_maxu_sci_b_instruction
		|| is_pv_srl_sci_h_instruction
		|| is_pv_srl_sci_b_instruction
		|| is_pv_sra_sci_h_instruction
		|| is_pv_sra_sci_b_instruction
		|| is_pv_sll_sci_h_instruction
		|| is_pv_sll_sci_b_instruction
		|| is_pv_or_sci_h_instruction
		|| is_pv_or_sci_b_instruction
		|| is_pv_xor_sci_h_instruction
		|| is_pv_xor_sci_b_instruction
		|| is_pv_and_sci_h_instruction
		|| is_pv_and_sci_b_instruction
		|| is_pv_extract_h_instruction
		|| is_pv_extract_b_instruction
		|| is_pv_extractu_h_instruction
		|| is_pv_extractu_b_instruction
		|| is_pv_insert_h_instruction
		|| is_pv_insert_b_instruction
		|| is_pv_dotup_sci_h_instruction
		|| is_pv_dotup_sci_b_instruction
		|| is_pv_dotusp_sci_h_instruction
		|| is_pv_dotusp_sci_b_instruction
		|| is_pv_dotsp_sci_h_instruction
		|| is_pv_dotsp_sci_b_instruction
		|| is_pv_sdotup_sci_h_instruction
		|| is_pv_sdotup_sci_b_instruction
		|| is_pv_sdotusp_sci_h_instruction
		|| is_pv_sdotusp_sci_b_instruction
		|| is_pv_sdotsp_sci_h_instruction
		|| is_pv_sdotsp_sci_b_instruction
		|| is_pv_shuffle_sci_h_instruction
		|| is_pv_shufflei0_sci_b_instruction
		|| is_pv_shufflei1_sci_b_instruction
		|| is_pv_shufflei2_sci_b_instruction
		|| is_pv_shufflei3_sci_b_instruction;
	assign has_operand_imm1 = 0
		|| is_p_beqimm_instruction
		|| is_p_bneimm_instruction;
	assign has_operand_imm2 = 0
		|| is_p_beqimm_instruction
		|| is_p_bneimm_instruction;
	assign has_operand_ls2 = 0
		|| is_p_extract_instruction
		|| is_p_extractu_instruction
		|| is_p_insert_instruction
		|| is_p_bclr_instruction
		|| is_p_bset_instruction;
	assign has_operand_ls3 = 0
		|| is_p_extract_instruction
		|| is_p_extractu_instruction
		|| is_p_insert_instruction
		|| is_p_bclr_instruction
		|| is_p_bset_instruction;
	assign has_operand_pred = 0
		|| is_fence_instruction;
	assign has_operand_rd = 0
		|| is_lui_instruction
		|| is_auipc_instruction
		|| is_jal_instruction
		|| is_jalr_instruction
		|| is_lb_instruction
		|| is_lh_instruction
		|| is_lw_instruction
		|| is_lbu_instruction
		|| is_lhu_instruction
		|| is_addi_instruction
		|| is_slti_instruction
		|| is_sltiu_instruction
		|| is_xori_instruction
		|| is_ori_instruction
		|| is_andi_instruction
		|| is_slli_instruction
		|| is_srli_instruction
		|| is_srai_instruction
		|| is_add_instruction
		|| is_sub_instruction
		|| is_sll_instruction
		|| is_slt_instruction
		|| is_sltu_instruction
		|| is_xor_instruction
		|| is_srl_instruction
		|| is_sra_instruction
		|| is_or_instruction
		|| is_and_instruction
		|| is_fence_instruction
		|| is_mul_instruction
		|| is_mulh_instruction
		|| is_mulhsu_instruction
		|| is_mulhu_instruction
		|| is_div_instruction
		|| is_divu_instruction
		|| is_rem_instruction
		|| is_remu_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction
		|| is_p_extract_instruction
		|| is_p_extractu_instruction
		|| is_p_insert_instruction
		|| is_p_bclr_instruction
		|| is_p_bset_instruction
		|| is_p_extractr_instruction
		|| is_p_extractur_instruction
		|| is_p_insertr_instruction
		|| is_p_bclrr_instruction
		|| is_p_bsetr_instruction
		|| is_p_ror_instruction
		|| is_p_ff1_instruction
		|| is_p_fl1_instruction
		|| is_p_clb_instruction
		|| is_p_cnt_instruction
		|| is_p_abs_instruction
		|| is_p_slet_instruction
		|| is_p_sletu_instruction
		|| is_p_min_instruction
		|| is_p_minu_instruction
		|| is_p_max_instruction
		|| is_p_maxu_instruction
		|| is_p_exths_instruction
		|| is_p_exthz_instruction
		|| is_p_extbs_instruction
		|| is_p_extbz_instruction
		|| is_p_clip_instruction
		|| is_p_clipu_instruction
		|| is_p_clipr_instruction
		|| is_p_clipur_instruction
		|| is_p_addn_instruction
		|| is_p_addun_instruction
		|| is_p_addrn_instruction
		|| is_p_addurn_instruction
		|| is_p_subn_instruction
		|| is_p_subun_instruction
		|| is_p_subrn_instruction
		|| is_p_suburn_instruction
		|| is_p_addnr_instruction
		|| is_p_addunr_instruction
		|| is_p_addrnr_instruction
		|| is_p_addurnr_instruction
		|| is_p_subnr_instruction
		|| is_p_subunr_instruction
		|| is_p_subrnr_instruction
		|| is_p_suburnr_instruction
		|| is_p_lb_i_inc_instruction
		|| is_p_lbu_i_inc_instruction
		|| is_p_lh_i_inc_instruction
		|| is_p_lhu_i_inc_instruction
		|| is_p_lw_i_inc_instruction
		|| is_p_lb_r_inc_instruction
		|| is_p_lbu_r_inc_instruction
		|| is_p_lh_r_inc_instruction
		|| is_p_lhu_r_inc_instruction
		|| is_p_lw_r_inc_instruction
		|| is_p_lb_r_instruction
		|| is_p_lbu_r_instruction
		|| is_p_lh_r_instruction
		|| is_p_lhu_r_instruction
		|| is_p_lw_r_instruction
		|| is_p_mulu_instruction
		|| is_p_mulhhu_instruction
		|| is_p_mulun_instruction
		|| is_p_mulhhun_instruction
		|| is_p_mulurn_instruction
		|| is_p_mulhhurn_instruction
		|| is_p_macsn_instruction
		|| is_p_machhsn_instruction
		|| is_p_macsrn_instruction
		|| is_p_machhsrn_instruction
		|| is_p_macun_instruction
		|| is_p_machhun_instruction
		|| is_p_macurn_instruction
		|| is_p_machhurn_instruction
		|| is_p_sb_r_inc_instruction
		|| is_p_sh_r_inc_instruction
		|| is_p_sw_r_inc_instruction
		|| is_p_sb_r_instruction
		|| is_p_sh_r_instruction
		|| is_p_sw_r_instruction
		|| is_pv_cmpeq_h_instruction
		|| is_pv_cmpeq_sc_h_instruction
		|| is_pv_cmpeq_sci_h_instruction
		|| is_pv_cmpeq_b_instruction
		|| is_pv_cmpeq_sc_b_instruction
		|| is_pv_cmpeq_sci_b_instruction
		|| is_pv_cmpne_h_instruction
		|| is_pv_cmpne_sc_h_instruction
		|| is_pv_cmpne_sci_h_instruction
		|| is_pv_cmpne_b_instruction
		|| is_pv_cmpne_sc_b_instruction
		|| is_pv_cmpne_sci_b_instruction
		|| is_pv_cmpgt_h_instruction
		|| is_pv_cmpgt_sc_h_instruction
		|| is_pv_cmpgt_sci_h_instruction
		|| is_pv_cmpgt_b_instruction
		|| is_pv_cmpgt_sc_b_instruction
		|| is_pv_cmpgt_sci_b_instruction
		|| is_pv_cmpge_h_instruction
		|| is_pv_cmpge_sc_h_instruction
		|| is_pv_cmpge_sci_h_instruction
		|| is_pv_cmpge_b_instruction
		|| is_pv_cmpge_sc_b_instruction
		|| is_pv_cmpge_sci_b_instruction
		|| is_pv_cmplt_h_instruction
		|| is_pv_cmplt_sc_h_instruction
		|| is_pv_cmplt_sci_h_instruction
		|| is_pv_cmplt_b_instruction
		|| is_pv_cmplt_sc_b_instruction
		|| is_pv_cmplt_sci_b_instruction
		|| is_pv_cmple_h_instruction
		|| is_pv_cmple_sc_h_instruction
		|| is_pv_cmple_sci_h_instruction
		|| is_pv_cmple_b_instruction
		|| is_pv_cmple_sc_b_instruction
		|| is_pv_cmple_sci_b_instruction
		|| is_pv_cmpgtu_h_instruction
		|| is_pv_cmpgtu_sc_h_instruction
		|| is_pv_cmpgtu_sci_h_instruction
		|| is_pv_cmpgtu_b_instruction
		|| is_pv_cmpgtu_sc_b_instruction
		|| is_pv_cmpgtu_sci_b_instruction
		|| is_pv_cmpgeu_h_instruction
		|| is_pv_cmpgeu_sc_h_instruction
		|| is_pv_cmpgeu_sci_h_instruction
		|| is_pv_cmpgeu_b_instruction
		|| is_pv_cmpgeu_sc_b_instruction
		|| is_pv_cmpgeu_sci_b_instruction
		|| is_pv_cmpltu_h_instruction
		|| is_pv_cmpltu_sc_h_instruction
		|| is_pv_cmpltu_sci_h_instruction
		|| is_pv_cmpltu_b_instruction
		|| is_pv_cmpltu_sc_b_instruction
		|| is_pv_cmpltu_sci_b_instruction
		|| is_pv_cmpleu_h_instruction
		|| is_pv_cmpleu_sc_h_instruction
		|| is_pv_cmpleu_sci_h_instruction
		|| is_pv_cmpleu_b_instruction
		|| is_pv_cmpleu_sc_b_instruction
		|| is_pv_cmpleu_sci_b_instruction
		|| is_pv_add_h_instruction
		|| is_pv_add_sc_h_instruction
		|| is_pv_add_sci_h_instruction
		|| is_pv_add_b_instruction
		|| is_pv_add_sc_b_instruction
		|| is_pv_add_sci_b_instruction
		|| is_pv_sub_h_instruction
		|| is_pv_sub_sc_h_instruction
		|| is_pv_sub_sci_h_instruction
		|| is_pv_sub_b_instruction
		|| is_pv_sub_sc_b_instruction
		|| is_pv_sub_sci_b_instruction
		|| is_pv_avg_h_instruction
		|| is_pv_avg_sc_h_instruction
		|| is_pv_avg_sci_h_instruction
		|| is_pv_avg_b_instruction
		|| is_pv_avg_sc_b_instruction
		|| is_pv_avg_sci_b_instruction
		|| is_pv_avgu_h_instruction
		|| is_pv_avgu_sc_h_instruction
		|| is_pv_avgu_sci_h_instruction
		|| is_pv_avgu_b_instruction
		|| is_pv_avgu_sc_b_instruction
		|| is_pv_avgu_sci_b_instruction
		|| is_pv_min_h_instruction
		|| is_pv_min_sc_h_instruction
		|| is_pv_min_sci_h_instruction
		|| is_pv_min_b_instruction
		|| is_pv_min_sc_b_instruction
		|| is_pv_min_sci_b_instruction
		|| is_pv_minu_h_instruction
		|| is_pv_minu_sc_h_instruction
		|| is_pv_minu_sci_h_instruction
		|| is_pv_minu_b_instruction
		|| is_pv_minu_sc_b_instruction
		|| is_pv_minu_sci_b_instruction
		|| is_pv_max_h_instruction
		|| is_pv_max_sc_h_instruction
		|| is_pv_max_sci_h_instruction
		|| is_pv_max_b_instruction
		|| is_pv_max_sc_b_instruction
		|| is_pv_max_sci_b_instruction
		|| is_pv_maxu_h_instruction
		|| is_pv_maxu_sc_h_instruction
		|| is_pv_maxu_sci_h_instruction
		|| is_pv_maxu_b_instruction
		|| is_pv_maxu_sc_b_instruction
		|| is_pv_maxu_sci_b_instruction
		|| is_pv_srl_h_instruction
		|| is_pv_srl_sc_h_instruction
		|| is_pv_srl_sci_h_instruction
		|| is_pv_srl_b_instruction
		|| is_pv_srl_sc_b_instruction
		|| is_pv_srl_sci_b_instruction
		|| is_pv_sra_h_instruction
		|| is_pv_sra_sc_h_instruction
		|| is_pv_sra_sci_h_instruction
		|| is_pv_sra_b_instruction
		|| is_pv_sra_sc_b_instruction
		|| is_pv_sra_sci_b_instruction
		|| is_pv_sll_h_instruction
		|| is_pv_sll_sc_h_instruction
		|| is_pv_sll_sci_h_instruction
		|| is_pv_sll_b_instruction
		|| is_pv_sll_sc_b_instruction
		|| is_pv_sll_sci_b_instruction
		|| is_pv_or_h_instruction
		|| is_pv_or_sc_h_instruction
		|| is_pv_or_sci_h_instruction
		|| is_pv_or_b_instruction
		|| is_pv_or_sc_b_instruction
		|| is_pv_or_sci_b_instruction
		|| is_pv_xor_h_instruction
		|| is_pv_xor_sc_h_instruction
		|| is_pv_xor_sci_h_instruction
		|| is_pv_xor_b_instruction
		|| is_pv_xor_sc_b_instruction
		|| is_pv_xor_sci_b_instruction
		|| is_pv_and_h_instruction
		|| is_pv_and_sc_h_instruction
		|| is_pv_and_sci_h_instruction
		|| is_pv_and_b_instruction
		|| is_pv_and_sc_b_instruction
		|| is_pv_and_sci_b_instruction
		|| is_pv_abs_h_instruction
		|| is_pv_abs_b_instruction
		|| is_pv_extract_h_instruction
		|| is_pv_extract_b_instruction
		|| is_pv_extractu_h_instruction
		|| is_pv_extractu_b_instruction
		|| is_pv_insert_h_instruction
		|| is_pv_insert_b_instruction
		|| is_pv_dotup_h_instruction
		|| is_pv_dotup_sc_h_instruction
		|| is_pv_dotup_sci_h_instruction
		|| is_pv_dotup_b_instruction
		|| is_pv_dotup_sc_b_instruction
		|| is_pv_dotup_sci_b_instruction
		|| is_pv_dotusp_h_instruction
		|| is_pv_dotusp_sc_h_instruction
		|| is_pv_dotusp_sci_h_instruction
		|| is_pv_dotusp_b_instruction
		|| is_pv_dotusp_sc_b_instruction
		|| is_pv_dotusp_sci_b_instruction
		|| is_pv_dotsp_h_instruction
		|| is_pv_dotsp_sc_h_instruction
		|| is_pv_dotsp_sci_h_instruction
		|| is_pv_dotsp_b_instruction
		|| is_pv_dotsp_sc_b_instruction
		|| is_pv_dotsp_sci_b_instruction
		|| is_pv_sdotup_h_instruction
		|| is_pv_sdotup_sc_h_instruction
		|| is_pv_sdotup_sci_h_instruction
		|| is_pv_sdotup_b_instruction
		|| is_pv_sdotup_sc_b_instruction
		|| is_pv_sdotup_sci_b_instruction
		|| is_pv_sdotusp_h_instruction
		|| is_pv_sdotusp_sc_h_instruction
		|| is_pv_sdotusp_sci_h_instruction
		|| is_pv_sdotusp_b_instruction
		|| is_pv_sdotusp_sc_b_instruction
		|| is_pv_sdotusp_sci_b_instruction
		|| is_pv_sdotsp_h_instruction
		|| is_pv_sdotsp_sc_h_instruction
		|| is_pv_sdotsp_sci_h_instruction
		|| is_pv_sdotsp_b_instruction
		|| is_pv_sdotsp_sc_b_instruction
		|| is_pv_sdotsp_sci_b_instruction
		|| is_pv_shuffle_h_instruction
		|| is_pv_shuffle_sci_h_instruction
		|| is_pv_shuffle_b_instruction
		|| is_pv_shufflei0_sci_b_instruction
		|| is_pv_shufflei1_sci_b_instruction
		|| is_pv_shufflei2_sci_b_instruction
		|| is_pv_shufflei3_sci_b_instruction
		|| is_pv_shuffle2_h_instruction
		|| is_pv_shuffle2_b_instruction
		|| is_pv_pack_h_instruction
		|| is_pv_packhi_b_instruction
		|| is_pv_packlo_b_instruction;
	assign has_operand_rs1 = 0
		|| is_jalr_instruction
		|| is_beq_instruction
		|| is_bne_instruction
		|| is_blt_instruction
		|| is_bge_instruction
		|| is_bltu_instruction
		|| is_bgeu_instruction
		|| is_lb_instruction
		|| is_lh_instruction
		|| is_lw_instruction
		|| is_lbu_instruction
		|| is_lhu_instruction
		|| is_sb_instruction
		|| is_sh_instruction
		|| is_sw_instruction
		|| is_addi_instruction
		|| is_slti_instruction
		|| is_sltiu_instruction
		|| is_xori_instruction
		|| is_ori_instruction
		|| is_andi_instruction
		|| is_slli_instruction
		|| is_srli_instruction
		|| is_srai_instruction
		|| is_add_instruction
		|| is_sub_instruction
		|| is_sll_instruction
		|| is_slt_instruction
		|| is_sltu_instruction
		|| is_xor_instruction
		|| is_srl_instruction
		|| is_sra_instruction
		|| is_or_instruction
		|| is_and_instruction
		|| is_fence_instruction
		|| is_mul_instruction
		|| is_mulh_instruction
		|| is_mulhsu_instruction
		|| is_mulhu_instruction
		|| is_div_instruction
		|| is_divu_instruction
		|| is_rem_instruction
		|| is_remu_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_p_extract_instruction
		|| is_p_extractu_instruction
		|| is_p_insert_instruction
		|| is_p_bclr_instruction
		|| is_p_bset_instruction
		|| is_p_extractr_instruction
		|| is_p_extractur_instruction
		|| is_p_insertr_instruction
		|| is_p_bclrr_instruction
		|| is_p_bsetr_instruction
		|| is_p_ror_instruction
		|| is_p_ff1_instruction
		|| is_p_fl1_instruction
		|| is_p_clb_instruction
		|| is_p_cnt_instruction
		|| is_p_abs_instruction
		|| is_p_slet_instruction
		|| is_p_sletu_instruction
		|| is_p_min_instruction
		|| is_p_minu_instruction
		|| is_p_max_instruction
		|| is_p_maxu_instruction
		|| is_p_exths_instruction
		|| is_p_exthz_instruction
		|| is_p_extbs_instruction
		|| is_p_extbz_instruction
		|| is_p_clip_instruction
		|| is_p_clipu_instruction
		|| is_p_clipr_instruction
		|| is_p_clipur_instruction
		|| is_p_addn_instruction
		|| is_p_addun_instruction
		|| is_p_addrn_instruction
		|| is_p_addurn_instruction
		|| is_p_subn_instruction
		|| is_p_subun_instruction
		|| is_p_subrn_instruction
		|| is_p_suburn_instruction
		|| is_p_addnr_instruction
		|| is_p_addunr_instruction
		|| is_p_addrnr_instruction
		|| is_p_addurnr_instruction
		|| is_p_subnr_instruction
		|| is_p_subunr_instruction
		|| is_p_subrnr_instruction
		|| is_p_suburnr_instruction
		|| is_lp_count_instruction
		|| is_lp_setup_instruction
		|| is_p_beqimm_instruction
		|| is_p_bneimm_instruction
		|| is_p_lb_i_inc_instruction
		|| is_p_lbu_i_inc_instruction
		|| is_p_lh_i_inc_instruction
		|| is_p_lhu_i_inc_instruction
		|| is_p_lw_i_inc_instruction
		|| is_p_lb_r_inc_instruction
		|| is_p_lbu_r_inc_instruction
		|| is_p_lh_r_inc_instruction
		|| is_p_lhu_r_inc_instruction
		|| is_p_lw_r_inc_instruction
		|| is_p_lb_r_instruction
		|| is_p_lbu_r_instruction
		|| is_p_lh_r_instruction
		|| is_p_lhu_r_instruction
		|| is_p_lw_r_instruction
		|| is_p_mulu_instruction
		|| is_p_mulhhu_instruction
		|| is_p_mulun_instruction
		|| is_p_mulhhun_instruction
		|| is_p_mulurn_instruction
		|| is_p_mulhhurn_instruction
		|| is_p_macsn_instruction
		|| is_p_machhsn_instruction
		|| is_p_macsrn_instruction
		|| is_p_machhsrn_instruction
		|| is_p_macun_instruction
		|| is_p_machhun_instruction
		|| is_p_macurn_instruction
		|| is_p_machhurn_instruction
		|| is_p_sb_i_inc_instruction
		|| is_p_sh_i_inc_instruction
		|| is_p_sw_i_inc_instruction
		|| is_p_sb_r_inc_instruction
		|| is_p_sh_r_inc_instruction
		|| is_p_sw_r_inc_instruction
		|| is_p_sb_r_instruction
		|| is_p_sh_r_instruction
		|| is_p_sw_r_instruction
		|| is_pv_cmpeq_h_instruction
		|| is_pv_cmpeq_sc_h_instruction
		|| is_pv_cmpeq_sci_h_instruction
		|| is_pv_cmpeq_b_instruction
		|| is_pv_cmpeq_sc_b_instruction
		|| is_pv_cmpeq_sci_b_instruction
		|| is_pv_cmpne_h_instruction
		|| is_pv_cmpne_sc_h_instruction
		|| is_pv_cmpne_sci_h_instruction
		|| is_pv_cmpne_b_instruction
		|| is_pv_cmpne_sc_b_instruction
		|| is_pv_cmpne_sci_b_instruction
		|| is_pv_cmpgt_h_instruction
		|| is_pv_cmpgt_sc_h_instruction
		|| is_pv_cmpgt_sci_h_instruction
		|| is_pv_cmpgt_b_instruction
		|| is_pv_cmpgt_sc_b_instruction
		|| is_pv_cmpgt_sci_b_instruction
		|| is_pv_cmpge_h_instruction
		|| is_pv_cmpge_sc_h_instruction
		|| is_pv_cmpge_sci_h_instruction
		|| is_pv_cmpge_b_instruction
		|| is_pv_cmpge_sc_b_instruction
		|| is_pv_cmpge_sci_b_instruction
		|| is_pv_cmplt_h_instruction
		|| is_pv_cmplt_sc_h_instruction
		|| is_pv_cmplt_sci_h_instruction
		|| is_pv_cmplt_b_instruction
		|| is_pv_cmplt_sc_b_instruction
		|| is_pv_cmplt_sci_b_instruction
		|| is_pv_cmple_h_instruction
		|| is_pv_cmple_sc_h_instruction
		|| is_pv_cmple_sci_h_instruction
		|| is_pv_cmple_b_instruction
		|| is_pv_cmple_sc_b_instruction
		|| is_pv_cmple_sci_b_instruction
		|| is_pv_cmpgtu_h_instruction
		|| is_pv_cmpgtu_sc_h_instruction
		|| is_pv_cmpgtu_sci_h_instruction
		|| is_pv_cmpgtu_b_instruction
		|| is_pv_cmpgtu_sc_b_instruction
		|| is_pv_cmpgtu_sci_b_instruction
		|| is_pv_cmpgeu_h_instruction
		|| is_pv_cmpgeu_sc_h_instruction
		|| is_pv_cmpgeu_sci_h_instruction
		|| is_pv_cmpgeu_b_instruction
		|| is_pv_cmpgeu_sc_b_instruction
		|| is_pv_cmpgeu_sci_b_instruction
		|| is_pv_cmpltu_h_instruction
		|| is_pv_cmpltu_sc_h_instruction
		|| is_pv_cmpltu_sci_h_instruction
		|| is_pv_cmpltu_b_instruction
		|| is_pv_cmpltu_sc_b_instruction
		|| is_pv_cmpltu_sci_b_instruction
		|| is_pv_cmpleu_h_instruction
		|| is_pv_cmpleu_sc_h_instruction
		|| is_pv_cmpleu_sci_h_instruction
		|| is_pv_cmpleu_b_instruction
		|| is_pv_cmpleu_sc_b_instruction
		|| is_pv_cmpleu_sci_b_instruction
		|| is_pv_add_h_instruction
		|| is_pv_add_sc_h_instruction
		|| is_pv_add_sci_h_instruction
		|| is_pv_add_b_instruction
		|| is_pv_add_sc_b_instruction
		|| is_pv_add_sci_b_instruction
		|| is_pv_sub_h_instruction
		|| is_pv_sub_sc_h_instruction
		|| is_pv_sub_sci_h_instruction
		|| is_pv_sub_b_instruction
		|| is_pv_sub_sc_b_instruction
		|| is_pv_sub_sci_b_instruction
		|| is_pv_avg_h_instruction
		|| is_pv_avg_sc_h_instruction
		|| is_pv_avg_sci_h_instruction
		|| is_pv_avg_b_instruction
		|| is_pv_avg_sc_b_instruction
		|| is_pv_avg_sci_b_instruction
		|| is_pv_avgu_h_instruction
		|| is_pv_avgu_sc_h_instruction
		|| is_pv_avgu_sci_h_instruction
		|| is_pv_avgu_b_instruction
		|| is_pv_avgu_sc_b_instruction
		|| is_pv_avgu_sci_b_instruction
		|| is_pv_min_h_instruction
		|| is_pv_min_sc_h_instruction
		|| is_pv_min_sci_h_instruction
		|| is_pv_min_b_instruction
		|| is_pv_min_sc_b_instruction
		|| is_pv_min_sci_b_instruction
		|| is_pv_minu_h_instruction
		|| is_pv_minu_sc_h_instruction
		|| is_pv_minu_sci_h_instruction
		|| is_pv_minu_b_instruction
		|| is_pv_minu_sc_b_instruction
		|| is_pv_minu_sci_b_instruction
		|| is_pv_max_h_instruction
		|| is_pv_max_sc_h_instruction
		|| is_pv_max_sci_h_instruction
		|| is_pv_max_b_instruction
		|| is_pv_max_sc_b_instruction
		|| is_pv_max_sci_b_instruction
		|| is_pv_maxu_h_instruction
		|| is_pv_maxu_sc_h_instruction
		|| is_pv_maxu_sci_h_instruction
		|| is_pv_maxu_b_instruction
		|| is_pv_maxu_sc_b_instruction
		|| is_pv_maxu_sci_b_instruction
		|| is_pv_srl_h_instruction
		|| is_pv_srl_sc_h_instruction
		|| is_pv_srl_sci_h_instruction
		|| is_pv_srl_b_instruction
		|| is_pv_srl_sc_b_instruction
		|| is_pv_srl_sci_b_instruction
		|| is_pv_sra_h_instruction
		|| is_pv_sra_sc_h_instruction
		|| is_pv_sra_sci_h_instruction
		|| is_pv_sra_b_instruction
		|| is_pv_sra_sc_b_instruction
		|| is_pv_sra_sci_b_instruction
		|| is_pv_sll_h_instruction
		|| is_pv_sll_sc_h_instruction
		|| is_pv_sll_sci_h_instruction
		|| is_pv_sll_b_instruction
		|| is_pv_sll_sc_b_instruction
		|| is_pv_sll_sci_b_instruction
		|| is_pv_or_h_instruction
		|| is_pv_or_sc_h_instruction
		|| is_pv_or_sci_h_instruction
		|| is_pv_or_b_instruction
		|| is_pv_or_sc_b_instruction
		|| is_pv_or_sci_b_instruction
		|| is_pv_xor_h_instruction
		|| is_pv_xor_sc_h_instruction
		|| is_pv_xor_sci_h_instruction
		|| is_pv_xor_b_instruction
		|| is_pv_xor_sc_b_instruction
		|| is_pv_xor_sci_b_instruction
		|| is_pv_and_h_instruction
		|| is_pv_and_sc_h_instruction
		|| is_pv_and_sci_h_instruction
		|| is_pv_and_b_instruction
		|| is_pv_and_sc_b_instruction
		|| is_pv_and_sci_b_instruction
		|| is_pv_abs_h_instruction
		|| is_pv_abs_b_instruction
		|| is_pv_extract_h_instruction
		|| is_pv_extract_b_instruction
		|| is_pv_extractu_h_instruction
		|| is_pv_extractu_b_instruction
		|| is_pv_insert_h_instruction
		|| is_pv_insert_b_instruction
		|| is_pv_dotup_h_instruction
		|| is_pv_dotup_sc_h_instruction
		|| is_pv_dotup_sci_h_instruction
		|| is_pv_dotup_b_instruction
		|| is_pv_dotup_sc_b_instruction
		|| is_pv_dotup_sci_b_instruction
		|| is_pv_dotusp_h_instruction
		|| is_pv_dotusp_sc_h_instruction
		|| is_pv_dotusp_sci_h_instruction
		|| is_pv_dotusp_b_instruction
		|| is_pv_dotusp_sc_b_instruction
		|| is_pv_dotusp_sci_b_instruction
		|| is_pv_dotsp_h_instruction
		|| is_pv_dotsp_sc_h_instruction
		|| is_pv_dotsp_sci_h_instruction
		|| is_pv_dotsp_b_instruction
		|| is_pv_dotsp_sc_b_instruction
		|| is_pv_dotsp_sci_b_instruction
		|| is_pv_sdotup_h_instruction
		|| is_pv_sdotup_sc_h_instruction
		|| is_pv_sdotup_sci_h_instruction
		|| is_pv_sdotup_b_instruction
		|| is_pv_sdotup_sc_b_instruction
		|| is_pv_sdotup_sci_b_instruction
		|| is_pv_sdotusp_h_instruction
		|| is_pv_sdotusp_sc_h_instruction
		|| is_pv_sdotusp_sci_h_instruction
		|| is_pv_sdotusp_b_instruction
		|| is_pv_sdotusp_sc_b_instruction
		|| is_pv_sdotusp_sci_b_instruction
		|| is_pv_sdotsp_h_instruction
		|| is_pv_sdotsp_sc_h_instruction
		|| is_pv_sdotsp_sci_h_instruction
		|| is_pv_sdotsp_b_instruction
		|| is_pv_sdotsp_sc_b_instruction
		|| is_pv_sdotsp_sci_b_instruction
		|| is_pv_shuffle_h_instruction
		|| is_pv_shuffle_sci_h_instruction
		|| is_pv_shuffle_b_instruction
		|| is_pv_shufflei0_sci_b_instruction
		|| is_pv_shufflei1_sci_b_instruction
		|| is_pv_shufflei2_sci_b_instruction
		|| is_pv_shufflei3_sci_b_instruction
		|| is_pv_shuffle2_h_instruction
		|| is_pv_shuffle2_b_instruction
		|| is_pv_pack_h_instruction
		|| is_pv_packhi_b_instruction
		|| is_pv_packlo_b_instruction;
	assign has_operand_rs2 = 0
		|| is_beq_instruction
		|| is_bne_instruction
		|| is_blt_instruction
		|| is_bge_instruction
		|| is_bltu_instruction
		|| is_bgeu_instruction
		|| is_sb_instruction
		|| is_sh_instruction
		|| is_sw_instruction
		|| is_add_instruction
		|| is_sub_instruction
		|| is_sll_instruction
		|| is_slt_instruction
		|| is_sltu_instruction
		|| is_xor_instruction
		|| is_srl_instruction
		|| is_sra_instruction
		|| is_or_instruction
		|| is_and_instruction
		|| is_mul_instruction
		|| is_mulh_instruction
		|| is_mulhsu_instruction
		|| is_mulhu_instruction
		|| is_div_instruction
		|| is_divu_instruction
		|| is_rem_instruction
		|| is_remu_instruction
		|| is_p_extractr_instruction
		|| is_p_extractur_instruction
		|| is_p_insertr_instruction
		|| is_p_bclrr_instruction
		|| is_p_bsetr_instruction
		|| is_p_ror_instruction
		|| is_p_slet_instruction
		|| is_p_sletu_instruction
		|| is_p_min_instruction
		|| is_p_minu_instruction
		|| is_p_max_instruction
		|| is_p_maxu_instruction
		|| is_p_clipr_instruction
		|| is_p_clipur_instruction
		|| is_p_addn_instruction
		|| is_p_addun_instruction
		|| is_p_addrn_instruction
		|| is_p_addurn_instruction
		|| is_p_subn_instruction
		|| is_p_subun_instruction
		|| is_p_subrn_instruction
		|| is_p_suburn_instruction
		|| is_p_addnr_instruction
		|| is_p_addunr_instruction
		|| is_p_addrnr_instruction
		|| is_p_addurnr_instruction
		|| is_p_subnr_instruction
		|| is_p_subunr_instruction
		|| is_p_subrnr_instruction
		|| is_p_suburnr_instruction
		|| is_p_lb_r_inc_instruction
		|| is_p_lbu_r_inc_instruction
		|| is_p_lh_r_inc_instruction
		|| is_p_lhu_r_inc_instruction
		|| is_p_lw_r_inc_instruction
		|| is_p_lb_r_instruction
		|| is_p_lbu_r_instruction
		|| is_p_lh_r_instruction
		|| is_p_lhu_r_instruction
		|| is_p_lw_r_instruction
		|| is_p_mulu_instruction
		|| is_p_mulhhu_instruction
		|| is_p_mulun_instruction
		|| is_p_mulhhun_instruction
		|| is_p_mulurn_instruction
		|| is_p_mulhhurn_instruction
		|| is_p_macsn_instruction
		|| is_p_machhsn_instruction
		|| is_p_macsrn_instruction
		|| is_p_machhsrn_instruction
		|| is_p_macun_instruction
		|| is_p_machhun_instruction
		|| is_p_macurn_instruction
		|| is_p_machhurn_instruction
		|| is_p_sb_i_inc_instruction
		|| is_p_sh_i_inc_instruction
		|| is_p_sw_i_inc_instruction
		|| is_p_sb_r_inc_instruction
		|| is_p_sh_r_inc_instruction
		|| is_p_sw_r_inc_instruction
		|| is_p_sb_r_instruction
		|| is_p_sh_r_instruction
		|| is_p_sw_r_instruction
		|| is_pv_cmpeq_h_instruction
		|| is_pv_cmpeq_sc_h_instruction
		|| is_pv_cmpeq_b_instruction
		|| is_pv_cmpeq_sc_b_instruction
		|| is_pv_cmpne_h_instruction
		|| is_pv_cmpne_sc_h_instruction
		|| is_pv_cmpne_b_instruction
		|| is_pv_cmpne_sc_b_instruction
		|| is_pv_cmpgt_h_instruction
		|| is_pv_cmpgt_sc_h_instruction
		|| is_pv_cmpgt_b_instruction
		|| is_pv_cmpgt_sc_b_instruction
		|| is_pv_cmpge_h_instruction
		|| is_pv_cmpge_sc_h_instruction
		|| is_pv_cmpge_b_instruction
		|| is_pv_cmpge_sc_b_instruction
		|| is_pv_cmplt_h_instruction
		|| is_pv_cmplt_sc_h_instruction
		|| is_pv_cmplt_b_instruction
		|| is_pv_cmplt_sc_b_instruction
		|| is_pv_cmple_h_instruction
		|| is_pv_cmple_sc_h_instruction
		|| is_pv_cmple_b_instruction
		|| is_pv_cmple_sc_b_instruction
		|| is_pv_cmpgtu_h_instruction
		|| is_pv_cmpgtu_sc_h_instruction
		|| is_pv_cmpgtu_b_instruction
		|| is_pv_cmpgtu_sc_b_instruction
		|| is_pv_cmpgeu_h_instruction
		|| is_pv_cmpgeu_sc_h_instruction
		|| is_pv_cmpgeu_b_instruction
		|| is_pv_cmpgeu_sc_b_instruction
		|| is_pv_cmpltu_h_instruction
		|| is_pv_cmpltu_sc_h_instruction
		|| is_pv_cmpltu_b_instruction
		|| is_pv_cmpltu_sc_b_instruction
		|| is_pv_cmpleu_h_instruction
		|| is_pv_cmpleu_sc_h_instruction
		|| is_pv_cmpleu_b_instruction
		|| is_pv_cmpleu_sc_b_instruction
		|| is_pv_add_h_instruction
		|| is_pv_add_sc_h_instruction
		|| is_pv_add_b_instruction
		|| is_pv_add_sc_b_instruction
		|| is_pv_sub_h_instruction
		|| is_pv_sub_sc_h_instruction
		|| is_pv_sub_b_instruction
		|| is_pv_sub_sc_b_instruction
		|| is_pv_avg_h_instruction
		|| is_pv_avg_sc_h_instruction
		|| is_pv_avg_b_instruction
		|| is_pv_avg_sc_b_instruction
		|| is_pv_avgu_h_instruction
		|| is_pv_avgu_sc_h_instruction
		|| is_pv_avgu_b_instruction
		|| is_pv_avgu_sc_b_instruction
		|| is_pv_min_h_instruction
		|| is_pv_min_sc_h_instruction
		|| is_pv_min_b_instruction
		|| is_pv_min_sc_b_instruction
		|| is_pv_minu_h_instruction
		|| is_pv_minu_sc_h_instruction
		|| is_pv_minu_b_instruction
		|| is_pv_minu_sc_b_instruction
		|| is_pv_max_h_instruction
		|| is_pv_max_sc_h_instruction
		|| is_pv_max_b_instruction
		|| is_pv_max_sc_b_instruction
		|| is_pv_maxu_h_instruction
		|| is_pv_maxu_sc_h_instruction
		|| is_pv_maxu_b_instruction
		|| is_pv_maxu_sc_b_instruction
		|| is_pv_srl_h_instruction
		|| is_pv_srl_sc_h_instruction
		|| is_pv_srl_b_instruction
		|| is_pv_srl_sc_b_instruction
		|| is_pv_sra_h_instruction
		|| is_pv_sra_sc_h_instruction
		|| is_pv_sra_b_instruction
		|| is_pv_sra_sc_b_instruction
		|| is_pv_sll_h_instruction
		|| is_pv_sll_sc_h_instruction
		|| is_pv_sll_b_instruction
		|| is_pv_sll_sc_b_instruction
		|| is_pv_or_h_instruction
		|| is_pv_or_sc_h_instruction
		|| is_pv_or_b_instruction
		|| is_pv_or_sc_b_instruction
		|| is_pv_xor_h_instruction
		|| is_pv_xor_sc_h_instruction
		|| is_pv_xor_b_instruction
		|| is_pv_xor_sc_b_instruction
		|| is_pv_and_h_instruction
		|| is_pv_and_sc_h_instruction
		|| is_pv_and_b_instruction
		|| is_pv_and_sc_b_instruction
		|| is_pv_dotup_h_instruction
		|| is_pv_dotup_sc_h_instruction
		|| is_pv_dotup_b_instruction
		|| is_pv_dotup_sc_b_instruction
		|| is_pv_dotusp_h_instruction
		|| is_pv_dotusp_sc_h_instruction
		|| is_pv_dotusp_b_instruction
		|| is_pv_dotusp_sc_b_instruction
		|| is_pv_dotsp_h_instruction
		|| is_pv_dotsp_sc_h_instruction
		|| is_pv_dotsp_b_instruction
		|| is_pv_dotsp_sc_b_instruction
		|| is_pv_sdotup_h_instruction
		|| is_pv_sdotup_sc_h_instruction
		|| is_pv_sdotup_b_instruction
		|| is_pv_sdotup_sc_b_instruction
		|| is_pv_sdotusp_h_instruction
		|| is_pv_sdotusp_sc_h_instruction
		|| is_pv_sdotusp_b_instruction
		|| is_pv_sdotusp_sc_b_instruction
		|| is_pv_sdotsp_h_instruction
		|| is_pv_sdotsp_sc_h_instruction
		|| is_pv_sdotsp_b_instruction
		|| is_pv_sdotsp_sc_b_instruction
		|| is_pv_shuffle_h_instruction
		|| is_pv_shuffle_b_instruction
		|| is_pv_shuffle2_h_instruction
		|| is_pv_shuffle2_b_instruction
		|| is_pv_pack_h_instruction
		|| is_pv_packhi_b_instruction
		|| is_pv_packlo_b_instruction;
	assign has_operand_shamt = 0
		|| is_slli_instruction
		|| is_srli_instruction
		|| is_srai_instruction;
	assign has_operand_succ = 0
		|| is_fence_instruction;
	assign has_operand_uimm = 0
		|| is_p_clip_instruction
		|| is_p_clipu_instruction
		|| is_p_addn_instruction
		|| is_p_addun_instruction
		|| is_p_addrn_instruction
		|| is_p_addurn_instruction
		|| is_p_subn_instruction
		|| is_p_subun_instruction
		|| is_p_subrn_instruction
		|| is_p_suburn_instruction
		|| is_p_addnr_instruction
		|| is_p_mulun_instruction
		|| is_p_mulhhun_instruction
		|| is_p_mulurn_instruction
		|| is_p_mulhhurn_instruction
		|| is_p_macsn_instruction
		|| is_p_machhsn_instruction
		|| is_p_macsrn_instruction
		|| is_p_machhsrn_instruction
		|| is_p_macun_instruction
		|| is_p_machhun_instruction
		|| is_p_macurn_instruction
		|| is_p_machhurn_instruction;
	assign has_operand_uimmL = 0
		|| is_lp_starti_instruction
		|| is_lp_endi_instruction
		|| is_lp_counti_instruction
		|| is_lp_setup_instruction
		|| is_lp_setupi_instruction;
	assign has_operand_uimmS = 0
		|| is_lp_setupi_instruction;
	assign has_operand_zimm = 0
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction;
	assign operand_rd =
		is_lui_instruction ? operand_rd_lui :
		is_auipc_instruction ? operand_rd_auipc :
		is_jal_instruction ? operand_rd_jal :
		is_jalr_instruction ? operand_rd_jalr :
		is_lb_instruction ? operand_rd_lb :
		is_lh_instruction ? operand_rd_lh :
		is_lw_instruction ? operand_rd_lw :
		is_lbu_instruction ? operand_rd_lbu :
		is_lhu_instruction ? operand_rd_lhu :
		is_addi_instruction ? operand_rd_addi :
		is_slti_instruction ? operand_rd_slti :
		is_sltiu_instruction ? operand_rd_sltiu :
		is_xori_instruction ? operand_rd_xori :
		is_ori_instruction ? operand_rd_ori :
		is_andi_instruction ? operand_rd_andi :
		is_slli_instruction ? operand_rd_slli :
		is_srli_instruction ? operand_rd_srli :
		is_srai_instruction ? operand_rd_srai :
		is_add_instruction ? operand_rd_add :
		is_sub_instruction ? operand_rd_sub :
		is_sll_instruction ? operand_rd_sll :
		is_slt_instruction ? operand_rd_slt :
		is_sltu_instruction ? operand_rd_sltu :
		is_xor_instruction ? operand_rd_xor :
		is_srl_instruction ? operand_rd_srl :
		is_sra_instruction ? operand_rd_sra :
		is_or_instruction ? operand_rd_or :
		is_and_instruction ? operand_rd_and :
		is_fence_instruction ? operand_rd_fence :
		is_mul_instruction ? operand_rd_mul :
		is_mulh_instruction ? operand_rd_mulh :
		is_mulhsu_instruction ? operand_rd_mulhsu :
		is_mulhu_instruction ? operand_rd_mulhu :
		is_div_instruction ? operand_rd_div :
		is_divu_instruction ? operand_rd_divu :
		is_rem_instruction ? operand_rd_rem :
		is_remu_instruction ? operand_rd_remu :
		is_csrrw_instruction ? operand_rd_csrrw :
		is_csrrs_instruction ? operand_rd_csrrs :
		is_csrrc_instruction ? operand_rd_csrrc :
		is_csrrwi_instruction ? operand_rd_csrrwi :
		is_csrrsi_instruction ? operand_rd_csrrsi :
		is_csrrci_instruction ? operand_rd_csrrci :
		is_p_extract_instruction ? operand_rd_p_extract :
		is_p_extractu_instruction ? operand_rd_p_extractu :
		is_p_insert_instruction ? operand_rd_p_insert :
		is_p_bclr_instruction ? operand_rd_p_bclr :
		is_p_bset_instruction ? operand_rd_p_bset :
		is_p_extractr_instruction ? operand_rd_p_extractr :
		is_p_extractur_instruction ? operand_rd_p_extractur :
		is_p_insertr_instruction ? operand_rd_p_insertr :
		is_p_bclrr_instruction ? operand_rd_p_bclrr :
		is_p_bsetr_instruction ? operand_rd_p_bsetr :
		is_p_ror_instruction ? operand_rd_p_ror :
		is_p_ff1_instruction ? operand_rd_p_ff1 :
		is_p_fl1_instruction ? operand_rd_p_fl1 :
		is_p_clb_instruction ? operand_rd_p_clb :
		is_p_cnt_instruction ? operand_rd_p_cnt :
		is_p_abs_instruction ? operand_rd_p_abs :
		is_p_slet_instruction ? operand_rd_p_slet :
		is_p_sletu_instruction ? operand_rd_p_sletu :
		is_p_min_instruction ? operand_rd_p_min :
		is_p_minu_instruction ? operand_rd_p_minu :
		is_p_max_instruction ? operand_rd_p_max :
		is_p_maxu_instruction ? operand_rd_p_maxu :
		is_p_exths_instruction ? operand_rd_p_exths :
		is_p_exthz_instruction ? operand_rd_p_exthz :
		is_p_extbs_instruction ? operand_rd_p_extbs :
		is_p_extbz_instruction ? operand_rd_p_extbz :
		is_p_clip_instruction ? operand_rd_p_clip :
		is_p_clipu_instruction ? operand_rd_p_clipu :
		is_p_clipr_instruction ? operand_rd_p_clipr :
		is_p_clipur_instruction ? operand_rd_p_clipur :
		is_p_addn_instruction ? operand_rd_p_addn :
		is_p_addun_instruction ? operand_rd_p_addun :
		is_p_addrn_instruction ? operand_rd_p_addrn :
		is_p_addurn_instruction ? operand_rd_p_addurn :
		is_p_subn_instruction ? operand_rd_p_subn :
		is_p_subun_instruction ? operand_rd_p_subun :
		is_p_subrn_instruction ? operand_rd_p_subrn :
		is_p_suburn_instruction ? operand_rd_p_suburn :
		is_p_addnr_instruction ? operand_rd_p_addnr :
		is_p_addunr_instruction ? operand_rd_p_addunr :
		is_p_addrnr_instruction ? operand_rd_p_addrnr :
		is_p_addurnr_instruction ? operand_rd_p_addurnr :
		is_p_subnr_instruction ? operand_rd_p_subnr :
		is_p_subunr_instruction ? operand_rd_p_subunr :
		is_p_subrnr_instruction ? operand_rd_p_subrnr :
		is_p_suburnr_instruction ? operand_rd_p_suburnr :
		is_p_lb_i_inc_instruction ? operand_rd_p_lb_i_inc :
		is_p_lbu_i_inc_instruction ? operand_rd_p_lbu_i_inc :
		is_p_lh_i_inc_instruction ? operand_rd_p_lh_i_inc :
		is_p_lhu_i_inc_instruction ? operand_rd_p_lhu_i_inc :
		is_p_lw_i_inc_instruction ? operand_rd_p_lw_i_inc :
		is_p_lb_r_inc_instruction ? operand_rd_p_lb_r_inc :
		is_p_lbu_r_inc_instruction ? operand_rd_p_lbu_r_inc :
		is_p_lh_r_inc_instruction ? operand_rd_p_lh_r_inc :
		is_p_lhu_r_inc_instruction ? operand_rd_p_lhu_r_inc :
		is_p_lw_r_inc_instruction ? operand_rd_p_lw_r_inc :
		is_p_lb_r_instruction ? operand_rd_p_lb_r :
		is_p_lbu_r_instruction ? operand_rd_p_lbu_r :
		is_p_lh_r_instruction ? operand_rd_p_lh_r :
		is_p_lhu_r_instruction ? operand_rd_p_lhu_r :
		is_p_lw_r_instruction ? operand_rd_p_lw_r :
		is_p_mulu_instruction ? operand_rd_p_mulu :
		is_p_mulhhu_instruction ? operand_rd_p_mulhhu :
		is_p_mulun_instruction ? operand_rd_p_mulun :
		is_p_mulhhun_instruction ? operand_rd_p_mulhhun :
		is_p_mulurn_instruction ? operand_rd_p_mulurn :
		is_p_mulhhurn_instruction ? operand_rd_p_mulhhurn :
		is_p_macsn_instruction ? operand_rd_p_macsn :
		is_p_machhsn_instruction ? operand_rd_p_machhsn :
		is_p_macsrn_instruction ? operand_rd_p_macsrn :
		is_p_machhsrn_instruction ? operand_rd_p_machhsrn :
		is_p_macun_instruction ? operand_rd_p_macun :
		is_p_machhun_instruction ? operand_rd_p_machhun :
		is_p_macurn_instruction ? operand_rd_p_macurn :
		is_p_machhurn_instruction ? operand_rd_p_machhurn :
		is_p_sb_r_inc_instruction ? operand_rd_p_sb_r_inc :
		is_p_sh_r_inc_instruction ? operand_rd_p_sh_r_inc :
		is_p_sw_r_inc_instruction ? operand_rd_p_sw_r_inc :
		is_p_sb_r_instruction ? operand_rd_p_sb_r :
		is_p_sh_r_instruction ? operand_rd_p_sh_r :
		is_p_sw_r_instruction ? operand_rd_p_sw_r :
		is_pv_cmpeq_h_instruction ? operand_rd_pv_cmpeq_h :
		is_pv_cmpeq_sc_h_instruction ? operand_rd_pv_cmpeq_sc_h :
		is_pv_cmpeq_sci_h_instruction ? operand_rd_pv_cmpeq_sci_h :
		is_pv_cmpeq_b_instruction ? operand_rd_pv_cmpeq_b :
		is_pv_cmpeq_sc_b_instruction ? operand_rd_pv_cmpeq_sc_b :
		is_pv_cmpeq_sci_b_instruction ? operand_rd_pv_cmpeq_sci_b :
		is_pv_cmpne_h_instruction ? operand_rd_pv_cmpne_h :
		is_pv_cmpne_sc_h_instruction ? operand_rd_pv_cmpne_sc_h :
		is_pv_cmpne_sci_h_instruction ? operand_rd_pv_cmpne_sci_h :
		is_pv_cmpne_b_instruction ? operand_rd_pv_cmpne_b :
		is_pv_cmpne_sc_b_instruction ? operand_rd_pv_cmpne_sc_b :
		is_pv_cmpne_sci_b_instruction ? operand_rd_pv_cmpne_sci_b :
		is_pv_cmpgt_h_instruction ? operand_rd_pv_cmpgt_h :
		is_pv_cmpgt_sc_h_instruction ? operand_rd_pv_cmpgt_sc_h :
		is_pv_cmpgt_sci_h_instruction ? operand_rd_pv_cmpgt_sci_h :
		is_pv_cmpgt_b_instruction ? operand_rd_pv_cmpgt_b :
		is_pv_cmpgt_sc_b_instruction ? operand_rd_pv_cmpgt_sc_b :
		is_pv_cmpgt_sci_b_instruction ? operand_rd_pv_cmpgt_sci_b :
		is_pv_cmpge_h_instruction ? operand_rd_pv_cmpge_h :
		is_pv_cmpge_sc_h_instruction ? operand_rd_pv_cmpge_sc_h :
		is_pv_cmpge_sci_h_instruction ? operand_rd_pv_cmpge_sci_h :
		is_pv_cmpge_b_instruction ? operand_rd_pv_cmpge_b :
		is_pv_cmpge_sc_b_instruction ? operand_rd_pv_cmpge_sc_b :
		is_pv_cmpge_sci_b_instruction ? operand_rd_pv_cmpge_sci_b :
		is_pv_cmplt_h_instruction ? operand_rd_pv_cmplt_h :
		is_pv_cmplt_sc_h_instruction ? operand_rd_pv_cmplt_sc_h :
		is_pv_cmplt_sci_h_instruction ? operand_rd_pv_cmplt_sci_h :
		is_pv_cmplt_b_instruction ? operand_rd_pv_cmplt_b :
		is_pv_cmplt_sc_b_instruction ? operand_rd_pv_cmplt_sc_b :
		is_pv_cmplt_sci_b_instruction ? operand_rd_pv_cmplt_sci_b :
		is_pv_cmple_h_instruction ? operand_rd_pv_cmple_h :
		is_pv_cmple_sc_h_instruction ? operand_rd_pv_cmple_sc_h :
		is_pv_cmple_sci_h_instruction ? operand_rd_pv_cmple_sci_h :
		is_pv_cmple_b_instruction ? operand_rd_pv_cmple_b :
		is_pv_cmple_sc_b_instruction ? operand_rd_pv_cmple_sc_b :
		is_pv_cmple_sci_b_instruction ? operand_rd_pv_cmple_sci_b :
		is_pv_cmpgtu_h_instruction ? operand_rd_pv_cmpgtu_h :
		is_pv_cmpgtu_sc_h_instruction ? operand_rd_pv_cmpgtu_sc_h :
		is_pv_cmpgtu_sci_h_instruction ? operand_rd_pv_cmpgtu_sci_h :
		is_pv_cmpgtu_b_instruction ? operand_rd_pv_cmpgtu_b :
		is_pv_cmpgtu_sc_b_instruction ? operand_rd_pv_cmpgtu_sc_b :
		is_pv_cmpgtu_sci_b_instruction ? operand_rd_pv_cmpgtu_sci_b :
		is_pv_cmpgeu_h_instruction ? operand_rd_pv_cmpgeu_h :
		is_pv_cmpgeu_sc_h_instruction ? operand_rd_pv_cmpgeu_sc_h :
		is_pv_cmpgeu_sci_h_instruction ? operand_rd_pv_cmpgeu_sci_h :
		is_pv_cmpgeu_b_instruction ? operand_rd_pv_cmpgeu_b :
		is_pv_cmpgeu_sc_b_instruction ? operand_rd_pv_cmpgeu_sc_b :
		is_pv_cmpgeu_sci_b_instruction ? operand_rd_pv_cmpgeu_sci_b :
		is_pv_cmpltu_h_instruction ? operand_rd_pv_cmpltu_h :
		is_pv_cmpltu_sc_h_instruction ? operand_rd_pv_cmpltu_sc_h :
		is_pv_cmpltu_sci_h_instruction ? operand_rd_pv_cmpltu_sci_h :
		is_pv_cmpltu_b_instruction ? operand_rd_pv_cmpltu_b :
		is_pv_cmpltu_sc_b_instruction ? operand_rd_pv_cmpltu_sc_b :
		is_pv_cmpltu_sci_b_instruction ? operand_rd_pv_cmpltu_sci_b :
		is_pv_cmpleu_h_instruction ? operand_rd_pv_cmpleu_h :
		is_pv_cmpleu_sc_h_instruction ? operand_rd_pv_cmpleu_sc_h :
		is_pv_cmpleu_sci_h_instruction ? operand_rd_pv_cmpleu_sci_h :
		is_pv_cmpleu_b_instruction ? operand_rd_pv_cmpleu_b :
		is_pv_cmpleu_sc_b_instruction ? operand_rd_pv_cmpleu_sc_b :
		is_pv_cmpleu_sci_b_instruction ? operand_rd_pv_cmpleu_sci_b :
		is_pv_add_h_instruction ? operand_rd_pv_add_h :
		is_pv_add_sc_h_instruction ? operand_rd_pv_add_sc_h :
		is_pv_add_sci_h_instruction ? operand_rd_pv_add_sci_h :
		is_pv_add_b_instruction ? operand_rd_pv_add_b :
		is_pv_add_sc_b_instruction ? operand_rd_pv_add_sc_b :
		is_pv_add_sci_b_instruction ? operand_rd_pv_add_sci_b :
		is_pv_sub_h_instruction ? operand_rd_pv_sub_h :
		is_pv_sub_sc_h_instruction ? operand_rd_pv_sub_sc_h :
		is_pv_sub_sci_h_instruction ? operand_rd_pv_sub_sci_h :
		is_pv_sub_b_instruction ? operand_rd_pv_sub_b :
		is_pv_sub_sc_b_instruction ? operand_rd_pv_sub_sc_b :
		is_pv_sub_sci_b_instruction ? operand_rd_pv_sub_sci_b :
		is_pv_avg_h_instruction ? operand_rd_pv_avg_h :
		is_pv_avg_sc_h_instruction ? operand_rd_pv_avg_sc_h :
		is_pv_avg_sci_h_instruction ? operand_rd_pv_avg_sci_h :
		is_pv_avg_b_instruction ? operand_rd_pv_avg_b :
		is_pv_avg_sc_b_instruction ? operand_rd_pv_avg_sc_b :
		is_pv_avg_sci_b_instruction ? operand_rd_pv_avg_sci_b :
		is_pv_avgu_h_instruction ? operand_rd_pv_avgu_h :
		is_pv_avgu_sc_h_instruction ? operand_rd_pv_avgu_sc_h :
		is_pv_avgu_sci_h_instruction ? operand_rd_pv_avgu_sci_h :
		is_pv_avgu_b_instruction ? operand_rd_pv_avgu_b :
		is_pv_avgu_sc_b_instruction ? operand_rd_pv_avgu_sc_b :
		is_pv_avgu_sci_b_instruction ? operand_rd_pv_avgu_sci_b :
		is_pv_min_h_instruction ? operand_rd_pv_min_h :
		is_pv_min_sc_h_instruction ? operand_rd_pv_min_sc_h :
		is_pv_min_sci_h_instruction ? operand_rd_pv_min_sci_h :
		is_pv_min_b_instruction ? operand_rd_pv_min_b :
		is_pv_min_sc_b_instruction ? operand_rd_pv_min_sc_b :
		is_pv_min_sci_b_instruction ? operand_rd_pv_min_sci_b :
		is_pv_minu_h_instruction ? operand_rd_pv_minu_h :
		is_pv_minu_sc_h_instruction ? operand_rd_pv_minu_sc_h :
		is_pv_minu_sci_h_instruction ? operand_rd_pv_minu_sci_h :
		is_pv_minu_b_instruction ? operand_rd_pv_minu_b :
		is_pv_minu_sc_b_instruction ? operand_rd_pv_minu_sc_b :
		is_pv_minu_sci_b_instruction ? operand_rd_pv_minu_sci_b :
		is_pv_max_h_instruction ? operand_rd_pv_max_h :
		is_pv_max_sc_h_instruction ? operand_rd_pv_max_sc_h :
		is_pv_max_sci_h_instruction ? operand_rd_pv_max_sci_h :
		is_pv_max_b_instruction ? operand_rd_pv_max_b :
		is_pv_max_sc_b_instruction ? operand_rd_pv_max_sc_b :
		is_pv_max_sci_b_instruction ? operand_rd_pv_max_sci_b :
		is_pv_maxu_h_instruction ? operand_rd_pv_maxu_h :
		is_pv_maxu_sc_h_instruction ? operand_rd_pv_maxu_sc_h :
		is_pv_maxu_sci_h_instruction ? operand_rd_pv_maxu_sci_h :
		is_pv_maxu_b_instruction ? operand_rd_pv_maxu_b :
		is_pv_maxu_sc_b_instruction ? operand_rd_pv_maxu_sc_b :
		is_pv_maxu_sci_b_instruction ? operand_rd_pv_maxu_sci_b :
		is_pv_srl_h_instruction ? operand_rd_pv_srl_h :
		is_pv_srl_sc_h_instruction ? operand_rd_pv_srl_sc_h :
		is_pv_srl_sci_h_instruction ? operand_rd_pv_srl_sci_h :
		is_pv_srl_b_instruction ? operand_rd_pv_srl_b :
		is_pv_srl_sc_b_instruction ? operand_rd_pv_srl_sc_b :
		is_pv_srl_sci_b_instruction ? operand_rd_pv_srl_sci_b :
		is_pv_sra_h_instruction ? operand_rd_pv_sra_h :
		is_pv_sra_sc_h_instruction ? operand_rd_pv_sra_sc_h :
		is_pv_sra_sci_h_instruction ? operand_rd_pv_sra_sci_h :
		is_pv_sra_b_instruction ? operand_rd_pv_sra_b :
		is_pv_sra_sc_b_instruction ? operand_rd_pv_sra_sc_b :
		is_pv_sra_sci_b_instruction ? operand_rd_pv_sra_sci_b :
		is_pv_sll_h_instruction ? operand_rd_pv_sll_h :
		is_pv_sll_sc_h_instruction ? operand_rd_pv_sll_sc_h :
		is_pv_sll_sci_h_instruction ? operand_rd_pv_sll_sci_h :
		is_pv_sll_b_instruction ? operand_rd_pv_sll_b :
		is_pv_sll_sc_b_instruction ? operand_rd_pv_sll_sc_b :
		is_pv_sll_sci_b_instruction ? operand_rd_pv_sll_sci_b :
		is_pv_or_h_instruction ? operand_rd_pv_or_h :
		is_pv_or_sc_h_instruction ? operand_rd_pv_or_sc_h :
		is_pv_or_sci_h_instruction ? operand_rd_pv_or_sci_h :
		is_pv_or_b_instruction ? operand_rd_pv_or_b :
		is_pv_or_sc_b_instruction ? operand_rd_pv_or_sc_b :
		is_pv_or_sci_b_instruction ? operand_rd_pv_or_sci_b :
		is_pv_xor_h_instruction ? operand_rd_pv_xor_h :
		is_pv_xor_sc_h_instruction ? operand_rd_pv_xor_sc_h :
		is_pv_xor_sci_h_instruction ? operand_rd_pv_xor_sci_h :
		is_pv_xor_b_instruction ? operand_rd_pv_xor_b :
		is_pv_xor_sc_b_instruction ? operand_rd_pv_xor_sc_b :
		is_pv_xor_sci_b_instruction ? operand_rd_pv_xor_sci_b :
		is_pv_and_h_instruction ? operand_rd_pv_and_h :
		is_pv_and_sc_h_instruction ? operand_rd_pv_and_sc_h :
		is_pv_and_sci_h_instruction ? operand_rd_pv_and_sci_h :
		is_pv_and_b_instruction ? operand_rd_pv_and_b :
		is_pv_and_sc_b_instruction ? operand_rd_pv_and_sc_b :
		is_pv_and_sci_b_instruction ? operand_rd_pv_and_sci_b :
		is_pv_abs_h_instruction ? operand_rd_pv_abs_h :
		is_pv_abs_b_instruction ? operand_rd_pv_abs_b :
		is_pv_extract_h_instruction ? operand_rd_pv_extract_h :
		is_pv_extract_b_instruction ? operand_rd_pv_extract_b :
		is_pv_extractu_h_instruction ? operand_rd_pv_extractu_h :
		is_pv_extractu_b_instruction ? operand_rd_pv_extractu_b :
		is_pv_insert_h_instruction ? operand_rd_pv_insert_h :
		is_pv_insert_b_instruction ? operand_rd_pv_insert_b :
		is_pv_dotup_h_instruction ? operand_rd_pv_dotup_h :
		is_pv_dotup_sc_h_instruction ? operand_rd_pv_dotup_sc_h :
		is_pv_dotup_sci_h_instruction ? operand_rd_pv_dotup_sci_h :
		is_pv_dotup_b_instruction ? operand_rd_pv_dotup_b :
		is_pv_dotup_sc_b_instruction ? operand_rd_pv_dotup_sc_b :
		is_pv_dotup_sci_b_instruction ? operand_rd_pv_dotup_sci_b :
		is_pv_dotusp_h_instruction ? operand_rd_pv_dotusp_h :
		is_pv_dotusp_sc_h_instruction ? operand_rd_pv_dotusp_sc_h :
		is_pv_dotusp_sci_h_instruction ? operand_rd_pv_dotusp_sci_h :
		is_pv_dotusp_b_instruction ? operand_rd_pv_dotusp_b :
		is_pv_dotusp_sc_b_instruction ? operand_rd_pv_dotusp_sc_b :
		is_pv_dotusp_sci_b_instruction ? operand_rd_pv_dotusp_sci_b :
		is_pv_dotsp_h_instruction ? operand_rd_pv_dotsp_h :
		is_pv_dotsp_sc_h_instruction ? operand_rd_pv_dotsp_sc_h :
		is_pv_dotsp_sci_h_instruction ? operand_rd_pv_dotsp_sci_h :
		is_pv_dotsp_b_instruction ? operand_rd_pv_dotsp_b :
		is_pv_dotsp_sc_b_instruction ? operand_rd_pv_dotsp_sc_b :
		is_pv_dotsp_sci_b_instruction ? operand_rd_pv_dotsp_sci_b :
		is_pv_sdotup_h_instruction ? operand_rd_pv_sdotup_h :
		is_pv_sdotup_sc_h_instruction ? operand_rd_pv_sdotup_sc_h :
		is_pv_sdotup_sci_h_instruction ? operand_rd_pv_sdotup_sci_h :
		is_pv_sdotup_b_instruction ? operand_rd_pv_sdotup_b :
		is_pv_sdotup_sc_b_instruction ? operand_rd_pv_sdotup_sc_b :
		is_pv_sdotup_sci_b_instruction ? operand_rd_pv_sdotup_sci_b :
		is_pv_sdotusp_h_instruction ? operand_rd_pv_sdotusp_h :
		is_pv_sdotusp_sc_h_instruction ? operand_rd_pv_sdotusp_sc_h :
		is_pv_sdotusp_sci_h_instruction ? operand_rd_pv_sdotusp_sci_h :
		is_pv_sdotusp_b_instruction ? operand_rd_pv_sdotusp_b :
		is_pv_sdotusp_sc_b_instruction ? operand_rd_pv_sdotusp_sc_b :
		is_pv_sdotusp_sci_b_instruction ? operand_rd_pv_sdotusp_sci_b :
		is_pv_sdotsp_h_instruction ? operand_rd_pv_sdotsp_h :
		is_pv_sdotsp_sc_h_instruction ? operand_rd_pv_sdotsp_sc_h :
		is_pv_sdotsp_sci_h_instruction ? operand_rd_pv_sdotsp_sci_h :
		is_pv_sdotsp_b_instruction ? operand_rd_pv_sdotsp_b :
		is_pv_sdotsp_sc_b_instruction ? operand_rd_pv_sdotsp_sc_b :
		is_pv_sdotsp_sci_b_instruction ? operand_rd_pv_sdotsp_sci_b :
		is_pv_shuffle_h_instruction ? operand_rd_pv_shuffle_h :
		is_pv_shuffle_sci_h_instruction ? operand_rd_pv_shuffle_sci_h :
		is_pv_shuffle_b_instruction ? operand_rd_pv_shuffle_b :
		is_pv_shufflei0_sci_b_instruction ? operand_rd_pv_shufflei0_sci_b :
		is_pv_shufflei1_sci_b_instruction ? operand_rd_pv_shufflei1_sci_b :
		is_pv_shufflei2_sci_b_instruction ? operand_rd_pv_shufflei2_sci_b :
		is_pv_shufflei3_sci_b_instruction ? operand_rd_pv_shufflei3_sci_b :
		is_pv_shuffle2_h_instruction ? operand_rd_pv_shuffle2_h :
		is_pv_shuffle2_b_instruction ? operand_rd_pv_shuffle2_b :
		is_pv_pack_h_instruction ? operand_rd_pv_pack_h :
		is_pv_packhi_b_instruction ? operand_rd_pv_packhi_b :
		is_pv_packlo_b_instruction ? operand_rd_pv_packlo_b :
		5'h 00;
	assign operand_rs1 =
		is_jalr_instruction ? operand_rs1_jalr :
		is_beq_instruction ? operand_rs1_beq :
		is_bne_instruction ? operand_rs1_bne :
		is_blt_instruction ? operand_rs1_blt :
		is_bge_instruction ? operand_rs1_bge :
		is_bltu_instruction ? operand_rs1_bltu :
		is_bgeu_instruction ? operand_rs1_bgeu :
		is_lb_instruction ? operand_rs1_lb :
		is_lh_instruction ? operand_rs1_lh :
		is_lw_instruction ? operand_rs1_lw :
		is_lbu_instruction ? operand_rs1_lbu :
		is_lhu_instruction ? operand_rs1_lhu :
		is_sb_instruction ? operand_rs1_sb :
		is_sh_instruction ? operand_rs1_sh :
		is_sw_instruction ? operand_rs1_sw :
		is_addi_instruction ? operand_rs1_addi :
		is_slti_instruction ? operand_rs1_slti :
		is_sltiu_instruction ? operand_rs1_sltiu :
		is_xori_instruction ? operand_rs1_xori :
		is_ori_instruction ? operand_rs1_ori :
		is_andi_instruction ? operand_rs1_andi :
		is_slli_instruction ? operand_rs1_slli :
		is_srli_instruction ? operand_rs1_srli :
		is_srai_instruction ? operand_rs1_srai :
		is_add_instruction ? operand_rs1_add :
		is_sub_instruction ? operand_rs1_sub :
		is_sll_instruction ? operand_rs1_sll :
		is_slt_instruction ? operand_rs1_slt :
		is_sltu_instruction ? operand_rs1_sltu :
		is_xor_instruction ? operand_rs1_xor :
		is_srl_instruction ? operand_rs1_srl :
		is_sra_instruction ? operand_rs1_sra :
		is_or_instruction ? operand_rs1_or :
		is_and_instruction ? operand_rs1_and :
		is_fence_instruction ? operand_rs1_fence :
		is_mul_instruction ? operand_rs1_mul :
		is_mulh_instruction ? operand_rs1_mulh :
		is_mulhsu_instruction ? operand_rs1_mulhsu :
		is_mulhu_instruction ? operand_rs1_mulhu :
		is_div_instruction ? operand_rs1_div :
		is_divu_instruction ? operand_rs1_divu :
		is_rem_instruction ? operand_rs1_rem :
		is_remu_instruction ? operand_rs1_remu :
		is_csrrw_instruction ? operand_rs1_csrrw :
		is_csrrs_instruction ? operand_rs1_csrrs :
		is_csrrc_instruction ? operand_rs1_csrrc :
		is_p_extract_instruction ? operand_rs1_p_extract :
		is_p_extractu_instruction ? operand_rs1_p_extractu :
		is_p_insert_instruction ? operand_rs1_p_insert :
		is_p_bclr_instruction ? operand_rs1_p_bclr :
		is_p_bset_instruction ? operand_rs1_p_bset :
		is_p_extractr_instruction ? operand_rs1_p_extractr :
		is_p_extractur_instruction ? operand_rs1_p_extractur :
		is_p_insertr_instruction ? operand_rs1_p_insertr :
		is_p_bclrr_instruction ? operand_rs1_p_bclrr :
		is_p_bsetr_instruction ? operand_rs1_p_bsetr :
		is_p_ror_instruction ? operand_rs1_p_ror :
		is_p_ff1_instruction ? operand_rs1_p_ff1 :
		is_p_fl1_instruction ? operand_rs1_p_fl1 :
		is_p_clb_instruction ? operand_rs1_p_clb :
		is_p_cnt_instruction ? operand_rs1_p_cnt :
		is_p_abs_instruction ? operand_rs1_p_abs :
		is_p_slet_instruction ? operand_rs1_p_slet :
		is_p_sletu_instruction ? operand_rs1_p_sletu :
		is_p_min_instruction ? operand_rs1_p_min :
		is_p_minu_instruction ? operand_rs1_p_minu :
		is_p_max_instruction ? operand_rs1_p_max :
		is_p_maxu_instruction ? operand_rs1_p_maxu :
		is_p_exths_instruction ? operand_rs1_p_exths :
		is_p_exthz_instruction ? operand_rs1_p_exthz :
		is_p_extbs_instruction ? operand_rs1_p_extbs :
		is_p_extbz_instruction ? operand_rs1_p_extbz :
		is_p_clip_instruction ? operand_rs1_p_clip :
		is_p_clipu_instruction ? operand_rs1_p_clipu :
		is_p_clipr_instruction ? operand_rs1_p_clipr :
		is_p_clipur_instruction ? operand_rs1_p_clipur :
		is_p_addn_instruction ? operand_rs1_p_addn :
		is_p_addun_instruction ? operand_rs1_p_addun :
		is_p_addrn_instruction ? operand_rs1_p_addrn :
		is_p_addurn_instruction ? operand_rs1_p_addurn :
		is_p_subn_instruction ? operand_rs1_p_subn :
		is_p_subun_instruction ? operand_rs1_p_subun :
		is_p_subrn_instruction ? operand_rs1_p_subrn :
		is_p_suburn_instruction ? operand_rs1_p_suburn :
		is_p_addnr_instruction ? operand_rs1_p_addnr :
		is_p_addunr_instruction ? operand_rs1_p_addunr :
		is_p_addrnr_instruction ? operand_rs1_p_addrnr :
		is_p_addurnr_instruction ? operand_rs1_p_addurnr :
		is_p_subnr_instruction ? operand_rs1_p_subnr :
		is_p_subunr_instruction ? operand_rs1_p_subunr :
		is_p_subrnr_instruction ? operand_rs1_p_subrnr :
		is_p_suburnr_instruction ? operand_rs1_p_suburnr :
		is_lp_count_instruction ? operand_rs1_lp_count :
		is_lp_setup_instruction ? operand_rs1_lp_setup :
		is_p_beqimm_instruction ? operand_rs1_p_beqimm :
		is_p_bneimm_instruction ? operand_rs1_p_bneimm :
		is_p_lb_i_inc_instruction ? operand_rs1_p_lb_i_inc :
		is_p_lbu_i_inc_instruction ? operand_rs1_p_lbu_i_inc :
		is_p_lh_i_inc_instruction ? operand_rs1_p_lh_i_inc :
		is_p_lhu_i_inc_instruction ? operand_rs1_p_lhu_i_inc :
		is_p_lw_i_inc_instruction ? operand_rs1_p_lw_i_inc :
		is_p_lb_r_inc_instruction ? operand_rs1_p_lb_r_inc :
		is_p_lbu_r_inc_instruction ? operand_rs1_p_lbu_r_inc :
		is_p_lh_r_inc_instruction ? operand_rs1_p_lh_r_inc :
		is_p_lhu_r_inc_instruction ? operand_rs1_p_lhu_r_inc :
		is_p_lw_r_inc_instruction ? operand_rs1_p_lw_r_inc :
		is_p_lb_r_instruction ? operand_rs1_p_lb_r :
		is_p_lbu_r_instruction ? operand_rs1_p_lbu_r :
		is_p_lh_r_instruction ? operand_rs1_p_lh_r :
		is_p_lhu_r_instruction ? operand_rs1_p_lhu_r :
		is_p_lw_r_instruction ? operand_rs1_p_lw_r :
		is_p_mulu_instruction ? operand_rs1_p_mulu :
		is_p_mulhhu_instruction ? operand_rs1_p_mulhhu :
		is_p_mulun_instruction ? operand_rs1_p_mulun :
		is_p_mulhhun_instruction ? operand_rs1_p_mulhhun :
		is_p_mulurn_instruction ? operand_rs1_p_mulurn :
		is_p_mulhhurn_instruction ? operand_rs1_p_mulhhurn :
		is_p_macsn_instruction ? operand_rs1_p_macsn :
		is_p_machhsn_instruction ? operand_rs1_p_machhsn :
		is_p_macsrn_instruction ? operand_rs1_p_macsrn :
		is_p_machhsrn_instruction ? operand_rs1_p_machhsrn :
		is_p_macun_instruction ? operand_rs1_p_macun :
		is_p_machhun_instruction ? operand_rs1_p_machhun :
		is_p_macurn_instruction ? operand_rs1_p_macurn :
		is_p_machhurn_instruction ? operand_rs1_p_machhurn :
		is_p_sb_i_inc_instruction ? operand_rs1_p_sb_i_inc :
		is_p_sh_i_inc_instruction ? operand_rs1_p_sh_i_inc :
		is_p_sw_i_inc_instruction ? operand_rs1_p_sw_i_inc :
		is_p_sb_r_inc_instruction ? operand_rs1_p_sb_r_inc :
		is_p_sh_r_inc_instruction ? operand_rs1_p_sh_r_inc :
		is_p_sw_r_inc_instruction ? operand_rs1_p_sw_r_inc :
		is_p_sb_r_instruction ? operand_rs1_p_sb_r :
		is_p_sh_r_instruction ? operand_rs1_p_sh_r :
		is_p_sw_r_instruction ? operand_rs1_p_sw_r :
		is_pv_cmpeq_h_instruction ? operand_rs1_pv_cmpeq_h :
		is_pv_cmpeq_sc_h_instruction ? operand_rs1_pv_cmpeq_sc_h :
		is_pv_cmpeq_sci_h_instruction ? operand_rs1_pv_cmpeq_sci_h :
		is_pv_cmpeq_b_instruction ? operand_rs1_pv_cmpeq_b :
		is_pv_cmpeq_sc_b_instruction ? operand_rs1_pv_cmpeq_sc_b :
		is_pv_cmpeq_sci_b_instruction ? operand_rs1_pv_cmpeq_sci_b :
		is_pv_cmpne_h_instruction ? operand_rs1_pv_cmpne_h :
		is_pv_cmpne_sc_h_instruction ? operand_rs1_pv_cmpne_sc_h :
		is_pv_cmpne_sci_h_instruction ? operand_rs1_pv_cmpne_sci_h :
		is_pv_cmpne_b_instruction ? operand_rs1_pv_cmpne_b :
		is_pv_cmpne_sc_b_instruction ? operand_rs1_pv_cmpne_sc_b :
		is_pv_cmpne_sci_b_instruction ? operand_rs1_pv_cmpne_sci_b :
		is_pv_cmpgt_h_instruction ? operand_rs1_pv_cmpgt_h :
		is_pv_cmpgt_sc_h_instruction ? operand_rs1_pv_cmpgt_sc_h :
		is_pv_cmpgt_sci_h_instruction ? operand_rs1_pv_cmpgt_sci_h :
		is_pv_cmpgt_b_instruction ? operand_rs1_pv_cmpgt_b :
		is_pv_cmpgt_sc_b_instruction ? operand_rs1_pv_cmpgt_sc_b :
		is_pv_cmpgt_sci_b_instruction ? operand_rs1_pv_cmpgt_sci_b :
		is_pv_cmpge_h_instruction ? operand_rs1_pv_cmpge_h :
		is_pv_cmpge_sc_h_instruction ? operand_rs1_pv_cmpge_sc_h :
		is_pv_cmpge_sci_h_instruction ? operand_rs1_pv_cmpge_sci_h :
		is_pv_cmpge_b_instruction ? operand_rs1_pv_cmpge_b :
		is_pv_cmpge_sc_b_instruction ? operand_rs1_pv_cmpge_sc_b :
		is_pv_cmpge_sci_b_instruction ? operand_rs1_pv_cmpge_sci_b :
		is_pv_cmplt_h_instruction ? operand_rs1_pv_cmplt_h :
		is_pv_cmplt_sc_h_instruction ? operand_rs1_pv_cmplt_sc_h :
		is_pv_cmplt_sci_h_instruction ? operand_rs1_pv_cmplt_sci_h :
		is_pv_cmplt_b_instruction ? operand_rs1_pv_cmplt_b :
		is_pv_cmplt_sc_b_instruction ? operand_rs1_pv_cmplt_sc_b :
		is_pv_cmplt_sci_b_instruction ? operand_rs1_pv_cmplt_sci_b :
		is_pv_cmple_h_instruction ? operand_rs1_pv_cmple_h :
		is_pv_cmple_sc_h_instruction ? operand_rs1_pv_cmple_sc_h :
		is_pv_cmple_sci_h_instruction ? operand_rs1_pv_cmple_sci_h :
		is_pv_cmple_b_instruction ? operand_rs1_pv_cmple_b :
		is_pv_cmple_sc_b_instruction ? operand_rs1_pv_cmple_sc_b :
		is_pv_cmple_sci_b_instruction ? operand_rs1_pv_cmple_sci_b :
		is_pv_cmpgtu_h_instruction ? operand_rs1_pv_cmpgtu_h :
		is_pv_cmpgtu_sc_h_instruction ? operand_rs1_pv_cmpgtu_sc_h :
		is_pv_cmpgtu_sci_h_instruction ? operand_rs1_pv_cmpgtu_sci_h :
		is_pv_cmpgtu_b_instruction ? operand_rs1_pv_cmpgtu_b :
		is_pv_cmpgtu_sc_b_instruction ? operand_rs1_pv_cmpgtu_sc_b :
		is_pv_cmpgtu_sci_b_instruction ? operand_rs1_pv_cmpgtu_sci_b :
		is_pv_cmpgeu_h_instruction ? operand_rs1_pv_cmpgeu_h :
		is_pv_cmpgeu_sc_h_instruction ? operand_rs1_pv_cmpgeu_sc_h :
		is_pv_cmpgeu_sci_h_instruction ? operand_rs1_pv_cmpgeu_sci_h :
		is_pv_cmpgeu_b_instruction ? operand_rs1_pv_cmpgeu_b :
		is_pv_cmpgeu_sc_b_instruction ? operand_rs1_pv_cmpgeu_sc_b :
		is_pv_cmpgeu_sci_b_instruction ? operand_rs1_pv_cmpgeu_sci_b :
		is_pv_cmpltu_h_instruction ? operand_rs1_pv_cmpltu_h :
		is_pv_cmpltu_sc_h_instruction ? operand_rs1_pv_cmpltu_sc_h :
		is_pv_cmpltu_sci_h_instruction ? operand_rs1_pv_cmpltu_sci_h :
		is_pv_cmpltu_b_instruction ? operand_rs1_pv_cmpltu_b :
		is_pv_cmpltu_sc_b_instruction ? operand_rs1_pv_cmpltu_sc_b :
		is_pv_cmpltu_sci_b_instruction ? operand_rs1_pv_cmpltu_sci_b :
		is_pv_cmpleu_h_instruction ? operand_rs1_pv_cmpleu_h :
		is_pv_cmpleu_sc_h_instruction ? operand_rs1_pv_cmpleu_sc_h :
		is_pv_cmpleu_sci_h_instruction ? operand_rs1_pv_cmpleu_sci_h :
		is_pv_cmpleu_b_instruction ? operand_rs1_pv_cmpleu_b :
		is_pv_cmpleu_sc_b_instruction ? operand_rs1_pv_cmpleu_sc_b :
		is_pv_cmpleu_sci_b_instruction ? operand_rs1_pv_cmpleu_sci_b :
		is_pv_add_h_instruction ? operand_rs1_pv_add_h :
		is_pv_add_sc_h_instruction ? operand_rs1_pv_add_sc_h :
		is_pv_add_sci_h_instruction ? operand_rs1_pv_add_sci_h :
		is_pv_add_b_instruction ? operand_rs1_pv_add_b :
		is_pv_add_sc_b_instruction ? operand_rs1_pv_add_sc_b :
		is_pv_add_sci_b_instruction ? operand_rs1_pv_add_sci_b :
		is_pv_sub_h_instruction ? operand_rs1_pv_sub_h :
		is_pv_sub_sc_h_instruction ? operand_rs1_pv_sub_sc_h :
		is_pv_sub_sci_h_instruction ? operand_rs1_pv_sub_sci_h :
		is_pv_sub_b_instruction ? operand_rs1_pv_sub_b :
		is_pv_sub_sc_b_instruction ? operand_rs1_pv_sub_sc_b :
		is_pv_sub_sci_b_instruction ? operand_rs1_pv_sub_sci_b :
		is_pv_avg_h_instruction ? operand_rs1_pv_avg_h :
		is_pv_avg_sc_h_instruction ? operand_rs1_pv_avg_sc_h :
		is_pv_avg_sci_h_instruction ? operand_rs1_pv_avg_sci_h :
		is_pv_avg_b_instruction ? operand_rs1_pv_avg_b :
		is_pv_avg_sc_b_instruction ? operand_rs1_pv_avg_sc_b :
		is_pv_avg_sci_b_instruction ? operand_rs1_pv_avg_sci_b :
		is_pv_avgu_h_instruction ? operand_rs1_pv_avgu_h :
		is_pv_avgu_sc_h_instruction ? operand_rs1_pv_avgu_sc_h :
		is_pv_avgu_sci_h_instruction ? operand_rs1_pv_avgu_sci_h :
		is_pv_avgu_b_instruction ? operand_rs1_pv_avgu_b :
		is_pv_avgu_sc_b_instruction ? operand_rs1_pv_avgu_sc_b :
		is_pv_avgu_sci_b_instruction ? operand_rs1_pv_avgu_sci_b :
		is_pv_min_h_instruction ? operand_rs1_pv_min_h :
		is_pv_min_sc_h_instruction ? operand_rs1_pv_min_sc_h :
		is_pv_min_sci_h_instruction ? operand_rs1_pv_min_sci_h :
		is_pv_min_b_instruction ? operand_rs1_pv_min_b :
		is_pv_min_sc_b_instruction ? operand_rs1_pv_min_sc_b :
		is_pv_min_sci_b_instruction ? operand_rs1_pv_min_sci_b :
		is_pv_minu_h_instruction ? operand_rs1_pv_minu_h :
		is_pv_minu_sc_h_instruction ? operand_rs1_pv_minu_sc_h :
		is_pv_minu_sci_h_instruction ? operand_rs1_pv_minu_sci_h :
		is_pv_minu_b_instruction ? operand_rs1_pv_minu_b :
		is_pv_minu_sc_b_instruction ? operand_rs1_pv_minu_sc_b :
		is_pv_minu_sci_b_instruction ? operand_rs1_pv_minu_sci_b :
		is_pv_max_h_instruction ? operand_rs1_pv_max_h :
		is_pv_max_sc_h_instruction ? operand_rs1_pv_max_sc_h :
		is_pv_max_sci_h_instruction ? operand_rs1_pv_max_sci_h :
		is_pv_max_b_instruction ? operand_rs1_pv_max_b :
		is_pv_max_sc_b_instruction ? operand_rs1_pv_max_sc_b :
		is_pv_max_sci_b_instruction ? operand_rs1_pv_max_sci_b :
		is_pv_maxu_h_instruction ? operand_rs1_pv_maxu_h :
		is_pv_maxu_sc_h_instruction ? operand_rs1_pv_maxu_sc_h :
		is_pv_maxu_sci_h_instruction ? operand_rs1_pv_maxu_sci_h :
		is_pv_maxu_b_instruction ? operand_rs1_pv_maxu_b :
		is_pv_maxu_sc_b_instruction ? operand_rs1_pv_maxu_sc_b :
		is_pv_maxu_sci_b_instruction ? operand_rs1_pv_maxu_sci_b :
		is_pv_srl_h_instruction ? operand_rs1_pv_srl_h :
		is_pv_srl_sc_h_instruction ? operand_rs1_pv_srl_sc_h :
		is_pv_srl_sci_h_instruction ? operand_rs1_pv_srl_sci_h :
		is_pv_srl_b_instruction ? operand_rs1_pv_srl_b :
		is_pv_srl_sc_b_instruction ? operand_rs1_pv_srl_sc_b :
		is_pv_srl_sci_b_instruction ? operand_rs1_pv_srl_sci_b :
		is_pv_sra_h_instruction ? operand_rs1_pv_sra_h :
		is_pv_sra_sc_h_instruction ? operand_rs1_pv_sra_sc_h :
		is_pv_sra_sci_h_instruction ? operand_rs1_pv_sra_sci_h :
		is_pv_sra_b_instruction ? operand_rs1_pv_sra_b :
		is_pv_sra_sc_b_instruction ? operand_rs1_pv_sra_sc_b :
		is_pv_sra_sci_b_instruction ? operand_rs1_pv_sra_sci_b :
		is_pv_sll_h_instruction ? operand_rs1_pv_sll_h :
		is_pv_sll_sc_h_instruction ? operand_rs1_pv_sll_sc_h :
		is_pv_sll_sci_h_instruction ? operand_rs1_pv_sll_sci_h :
		is_pv_sll_b_instruction ? operand_rs1_pv_sll_b :
		is_pv_sll_sc_b_instruction ? operand_rs1_pv_sll_sc_b :
		is_pv_sll_sci_b_instruction ? operand_rs1_pv_sll_sci_b :
		is_pv_or_h_instruction ? operand_rs1_pv_or_h :
		is_pv_or_sc_h_instruction ? operand_rs1_pv_or_sc_h :
		is_pv_or_sci_h_instruction ? operand_rs1_pv_or_sci_h :
		is_pv_or_b_instruction ? operand_rs1_pv_or_b :
		is_pv_or_sc_b_instruction ? operand_rs1_pv_or_sc_b :
		is_pv_or_sci_b_instruction ? operand_rs1_pv_or_sci_b :
		is_pv_xor_h_instruction ? operand_rs1_pv_xor_h :
		is_pv_xor_sc_h_instruction ? operand_rs1_pv_xor_sc_h :
		is_pv_xor_sci_h_instruction ? operand_rs1_pv_xor_sci_h :
		is_pv_xor_b_instruction ? operand_rs1_pv_xor_b :
		is_pv_xor_sc_b_instruction ? operand_rs1_pv_xor_sc_b :
		is_pv_xor_sci_b_instruction ? operand_rs1_pv_xor_sci_b :
		is_pv_and_h_instruction ? operand_rs1_pv_and_h :
		is_pv_and_sc_h_instruction ? operand_rs1_pv_and_sc_h :
		is_pv_and_sci_h_instruction ? operand_rs1_pv_and_sci_h :
		is_pv_and_b_instruction ? operand_rs1_pv_and_b :
		is_pv_and_sc_b_instruction ? operand_rs1_pv_and_sc_b :
		is_pv_and_sci_b_instruction ? operand_rs1_pv_and_sci_b :
		is_pv_abs_h_instruction ? operand_rs1_pv_abs_h :
		is_pv_abs_b_instruction ? operand_rs1_pv_abs_b :
		is_pv_extract_h_instruction ? operand_rs1_pv_extract_h :
		is_pv_extract_b_instruction ? operand_rs1_pv_extract_b :
		is_pv_extractu_h_instruction ? operand_rs1_pv_extractu_h :
		is_pv_extractu_b_instruction ? operand_rs1_pv_extractu_b :
		is_pv_insert_h_instruction ? operand_rs1_pv_insert_h :
		is_pv_insert_b_instruction ? operand_rs1_pv_insert_b :
		is_pv_dotup_h_instruction ? operand_rs1_pv_dotup_h :
		is_pv_dotup_sc_h_instruction ? operand_rs1_pv_dotup_sc_h :
		is_pv_dotup_sci_h_instruction ? operand_rs1_pv_dotup_sci_h :
		is_pv_dotup_b_instruction ? operand_rs1_pv_dotup_b :
		is_pv_dotup_sc_b_instruction ? operand_rs1_pv_dotup_sc_b :
		is_pv_dotup_sci_b_instruction ? operand_rs1_pv_dotup_sci_b :
		is_pv_dotusp_h_instruction ? operand_rs1_pv_dotusp_h :
		is_pv_dotusp_sc_h_instruction ? operand_rs1_pv_dotusp_sc_h :
		is_pv_dotusp_sci_h_instruction ? operand_rs1_pv_dotusp_sci_h :
		is_pv_dotusp_b_instruction ? operand_rs1_pv_dotusp_b :
		is_pv_dotusp_sc_b_instruction ? operand_rs1_pv_dotusp_sc_b :
		is_pv_dotusp_sci_b_instruction ? operand_rs1_pv_dotusp_sci_b :
		is_pv_dotsp_h_instruction ? operand_rs1_pv_dotsp_h :
		is_pv_dotsp_sc_h_instruction ? operand_rs1_pv_dotsp_sc_h :
		is_pv_dotsp_sci_h_instruction ? operand_rs1_pv_dotsp_sci_h :
		is_pv_dotsp_b_instruction ? operand_rs1_pv_dotsp_b :
		is_pv_dotsp_sc_b_instruction ? operand_rs1_pv_dotsp_sc_b :
		is_pv_dotsp_sci_b_instruction ? operand_rs1_pv_dotsp_sci_b :
		is_pv_sdotup_h_instruction ? operand_rs1_pv_sdotup_h :
		is_pv_sdotup_sc_h_instruction ? operand_rs1_pv_sdotup_sc_h :
		is_pv_sdotup_sci_h_instruction ? operand_rs1_pv_sdotup_sci_h :
		is_pv_sdotup_b_instruction ? operand_rs1_pv_sdotup_b :
		is_pv_sdotup_sc_b_instruction ? operand_rs1_pv_sdotup_sc_b :
		is_pv_sdotup_sci_b_instruction ? operand_rs1_pv_sdotup_sci_b :
		is_pv_sdotusp_h_instruction ? operand_rs1_pv_sdotusp_h :
		is_pv_sdotusp_sc_h_instruction ? operand_rs1_pv_sdotusp_sc_h :
		is_pv_sdotusp_sci_h_instruction ? operand_rs1_pv_sdotusp_sci_h :
		is_pv_sdotusp_b_instruction ? operand_rs1_pv_sdotusp_b :
		is_pv_sdotusp_sc_b_instruction ? operand_rs1_pv_sdotusp_sc_b :
		is_pv_sdotusp_sci_b_instruction ? operand_rs1_pv_sdotusp_sci_b :
		is_pv_sdotsp_h_instruction ? operand_rs1_pv_sdotsp_h :
		is_pv_sdotsp_sc_h_instruction ? operand_rs1_pv_sdotsp_sc_h :
		is_pv_sdotsp_sci_h_instruction ? operand_rs1_pv_sdotsp_sci_h :
		is_pv_sdotsp_b_instruction ? operand_rs1_pv_sdotsp_b :
		is_pv_sdotsp_sc_b_instruction ? operand_rs1_pv_sdotsp_sc_b :
		is_pv_sdotsp_sci_b_instruction ? operand_rs1_pv_sdotsp_sci_b :
		is_pv_shuffle_h_instruction ? operand_rs1_pv_shuffle_h :
		is_pv_shuffle_sci_h_instruction ? operand_rs1_pv_shuffle_sci_h :
		is_pv_shuffle_b_instruction ? operand_rs1_pv_shuffle_b :
		is_pv_shufflei0_sci_b_instruction ? operand_rs1_pv_shufflei0_sci_b :
		is_pv_shufflei1_sci_b_instruction ? operand_rs1_pv_shufflei1_sci_b :
		is_pv_shufflei2_sci_b_instruction ? operand_rs1_pv_shufflei2_sci_b :
		is_pv_shufflei3_sci_b_instruction ? operand_rs1_pv_shufflei3_sci_b :
		is_pv_shuffle2_h_instruction ? operand_rs1_pv_shuffle2_h :
		is_pv_shuffle2_b_instruction ? operand_rs1_pv_shuffle2_b :
		is_pv_pack_h_instruction ? operand_rs1_pv_pack_h :
		is_pv_packhi_b_instruction ? operand_rs1_pv_packhi_b :
		is_pv_packlo_b_instruction ? operand_rs1_pv_packlo_b :
		5'h 00;
	assign operand_rs2 =
		is_beq_instruction ? operand_rs2_beq :
		is_bne_instruction ? operand_rs2_bne :
		is_blt_instruction ? operand_rs2_blt :
		is_bge_instruction ? operand_rs2_bge :
		is_bltu_instruction ? operand_rs2_bltu :
		is_bgeu_instruction ? operand_rs2_bgeu :
		is_sb_instruction ? operand_rs2_sb :
		is_sh_instruction ? operand_rs2_sh :
		is_sw_instruction ? operand_rs2_sw :
		is_add_instruction ? operand_rs2_add :
		is_sub_instruction ? operand_rs2_sub :
		is_sll_instruction ? operand_rs2_sll :
		is_slt_instruction ? operand_rs2_slt :
		is_sltu_instruction ? operand_rs2_sltu :
		is_xor_instruction ? operand_rs2_xor :
		is_srl_instruction ? operand_rs2_srl :
		is_sra_instruction ? operand_rs2_sra :
		is_or_instruction ? operand_rs2_or :
		is_and_instruction ? operand_rs2_and :
		is_mul_instruction ? operand_rs2_mul :
		is_mulh_instruction ? operand_rs2_mulh :
		is_mulhsu_instruction ? operand_rs2_mulhsu :
		is_mulhu_instruction ? operand_rs2_mulhu :
		is_div_instruction ? operand_rs2_div :
		is_divu_instruction ? operand_rs2_divu :
		is_rem_instruction ? operand_rs2_rem :
		is_remu_instruction ? operand_rs2_remu :
		is_p_extractr_instruction ? operand_rs2_p_extractr :
		is_p_extractur_instruction ? operand_rs2_p_extractur :
		is_p_insertr_instruction ? operand_rs2_p_insertr :
		is_p_bclrr_instruction ? operand_rs2_p_bclrr :
		is_p_bsetr_instruction ? operand_rs2_p_bsetr :
		is_p_ror_instruction ? operand_rs2_p_ror :
		is_p_slet_instruction ? operand_rs2_p_slet :
		is_p_sletu_instruction ? operand_rs2_p_sletu :
		is_p_min_instruction ? operand_rs2_p_min :
		is_p_minu_instruction ? operand_rs2_p_minu :
		is_p_max_instruction ? operand_rs2_p_max :
		is_p_maxu_instruction ? operand_rs2_p_maxu :
		is_p_clipr_instruction ? operand_rs2_p_clipr :
		is_p_clipur_instruction ? operand_rs2_p_clipur :
		is_p_addn_instruction ? operand_rs2_p_addn :
		is_p_addun_instruction ? operand_rs2_p_addun :
		is_p_addrn_instruction ? operand_rs2_p_addrn :
		is_p_addurn_instruction ? operand_rs2_p_addurn :
		is_p_subn_instruction ? operand_rs2_p_subn :
		is_p_subun_instruction ? operand_rs2_p_subun :
		is_p_subrn_instruction ? operand_rs2_p_subrn :
		is_p_suburn_instruction ? operand_rs2_p_suburn :
		is_p_addnr_instruction ? operand_rs2_p_addnr :
		is_p_addunr_instruction ? operand_rs2_p_addunr :
		is_p_addrnr_instruction ? operand_rs2_p_addrnr :
		is_p_addurnr_instruction ? operand_rs2_p_addurnr :
		is_p_subnr_instruction ? operand_rs2_p_subnr :
		is_p_subunr_instruction ? operand_rs2_p_subunr :
		is_p_subrnr_instruction ? operand_rs2_p_subrnr :
		is_p_suburnr_instruction ? operand_rs2_p_suburnr :
		is_p_lb_r_inc_instruction ? operand_rs2_p_lb_r_inc :
		is_p_lbu_r_inc_instruction ? operand_rs2_p_lbu_r_inc :
		is_p_lh_r_inc_instruction ? operand_rs2_p_lh_r_inc :
		is_p_lhu_r_inc_instruction ? operand_rs2_p_lhu_r_inc :
		is_p_lw_r_inc_instruction ? operand_rs2_p_lw_r_inc :
		is_p_lb_r_instruction ? operand_rs2_p_lb_r :
		is_p_lbu_r_instruction ? operand_rs2_p_lbu_r :
		is_p_lh_r_instruction ? operand_rs2_p_lh_r :
		is_p_lhu_r_instruction ? operand_rs2_p_lhu_r :
		is_p_lw_r_instruction ? operand_rs2_p_lw_r :
		is_p_mulu_instruction ? operand_rs2_p_mulu :
		is_p_mulhhu_instruction ? operand_rs2_p_mulhhu :
		is_p_mulun_instruction ? operand_rs2_p_mulun :
		is_p_mulhhun_instruction ? operand_rs2_p_mulhhun :
		is_p_mulurn_instruction ? operand_rs2_p_mulurn :
		is_p_mulhhurn_instruction ? operand_rs2_p_mulhhurn :
		is_p_macsn_instruction ? operand_rs2_p_macsn :
		is_p_machhsn_instruction ? operand_rs2_p_machhsn :
		is_p_macsrn_instruction ? operand_rs2_p_macsrn :
		is_p_machhsrn_instruction ? operand_rs2_p_machhsrn :
		is_p_macun_instruction ? operand_rs2_p_macun :
		is_p_machhun_instruction ? operand_rs2_p_machhun :
		is_p_macurn_instruction ? operand_rs2_p_macurn :
		is_p_machhurn_instruction ? operand_rs2_p_machhurn :
		is_p_sb_i_inc_instruction ? operand_rs2_p_sb_i_inc :
		is_p_sh_i_inc_instruction ? operand_rs2_p_sh_i_inc :
		is_p_sw_i_inc_instruction ? operand_rs2_p_sw_i_inc :
		is_p_sb_r_inc_instruction ? operand_rs2_p_sb_r_inc :
		is_p_sh_r_inc_instruction ? operand_rs2_p_sh_r_inc :
		is_p_sw_r_inc_instruction ? operand_rs2_p_sw_r_inc :
		is_p_sb_r_instruction ? operand_rs2_p_sb_r :
		is_p_sh_r_instruction ? operand_rs2_p_sh_r :
		is_p_sw_r_instruction ? operand_rs2_p_sw_r :
		is_pv_cmpeq_h_instruction ? operand_rs2_pv_cmpeq_h :
		is_pv_cmpeq_sc_h_instruction ? operand_rs2_pv_cmpeq_sc_h :
		is_pv_cmpeq_b_instruction ? operand_rs2_pv_cmpeq_b :
		is_pv_cmpeq_sc_b_instruction ? operand_rs2_pv_cmpeq_sc_b :
		is_pv_cmpne_h_instruction ? operand_rs2_pv_cmpne_h :
		is_pv_cmpne_sc_h_instruction ? operand_rs2_pv_cmpne_sc_h :
		is_pv_cmpne_b_instruction ? operand_rs2_pv_cmpne_b :
		is_pv_cmpne_sc_b_instruction ? operand_rs2_pv_cmpne_sc_b :
		is_pv_cmpgt_h_instruction ? operand_rs2_pv_cmpgt_h :
		is_pv_cmpgt_sc_h_instruction ? operand_rs2_pv_cmpgt_sc_h :
		is_pv_cmpgt_b_instruction ? operand_rs2_pv_cmpgt_b :
		is_pv_cmpgt_sc_b_instruction ? operand_rs2_pv_cmpgt_sc_b :
		is_pv_cmpge_h_instruction ? operand_rs2_pv_cmpge_h :
		is_pv_cmpge_sc_h_instruction ? operand_rs2_pv_cmpge_sc_h :
		is_pv_cmpge_b_instruction ? operand_rs2_pv_cmpge_b :
		is_pv_cmpge_sc_b_instruction ? operand_rs2_pv_cmpge_sc_b :
		is_pv_cmplt_h_instruction ? operand_rs2_pv_cmplt_h :
		is_pv_cmplt_sc_h_instruction ? operand_rs2_pv_cmplt_sc_h :
		is_pv_cmplt_b_instruction ? operand_rs2_pv_cmplt_b :
		is_pv_cmplt_sc_b_instruction ? operand_rs2_pv_cmplt_sc_b :
		is_pv_cmple_h_instruction ? operand_rs2_pv_cmple_h :
		is_pv_cmple_sc_h_instruction ? operand_rs2_pv_cmple_sc_h :
		is_pv_cmple_b_instruction ? operand_rs2_pv_cmple_b :
		is_pv_cmple_sc_b_instruction ? operand_rs2_pv_cmple_sc_b :
		is_pv_cmpgtu_h_instruction ? operand_rs2_pv_cmpgtu_h :
		is_pv_cmpgtu_sc_h_instruction ? operand_rs2_pv_cmpgtu_sc_h :
		is_pv_cmpgtu_b_instruction ? operand_rs2_pv_cmpgtu_b :
		is_pv_cmpgtu_sc_b_instruction ? operand_rs2_pv_cmpgtu_sc_b :
		is_pv_cmpgeu_h_instruction ? operand_rs2_pv_cmpgeu_h :
		is_pv_cmpgeu_sc_h_instruction ? operand_rs2_pv_cmpgeu_sc_h :
		is_pv_cmpgeu_b_instruction ? operand_rs2_pv_cmpgeu_b :
		is_pv_cmpgeu_sc_b_instruction ? operand_rs2_pv_cmpgeu_sc_b :
		is_pv_cmpltu_h_instruction ? operand_rs2_pv_cmpltu_h :
		is_pv_cmpltu_sc_h_instruction ? operand_rs2_pv_cmpltu_sc_h :
		is_pv_cmpltu_b_instruction ? operand_rs2_pv_cmpltu_b :
		is_pv_cmpltu_sc_b_instruction ? operand_rs2_pv_cmpltu_sc_b :
		is_pv_cmpleu_h_instruction ? operand_rs2_pv_cmpleu_h :
		is_pv_cmpleu_sc_h_instruction ? operand_rs2_pv_cmpleu_sc_h :
		is_pv_cmpleu_b_instruction ? operand_rs2_pv_cmpleu_b :
		is_pv_cmpleu_sc_b_instruction ? operand_rs2_pv_cmpleu_sc_b :
		is_pv_add_h_instruction ? operand_rs2_pv_add_h :
		is_pv_add_sc_h_instruction ? operand_rs2_pv_add_sc_h :
		is_pv_add_b_instruction ? operand_rs2_pv_add_b :
		is_pv_add_sc_b_instruction ? operand_rs2_pv_add_sc_b :
		is_pv_sub_h_instruction ? operand_rs2_pv_sub_h :
		is_pv_sub_sc_h_instruction ? operand_rs2_pv_sub_sc_h :
		is_pv_sub_b_instruction ? operand_rs2_pv_sub_b :
		is_pv_sub_sc_b_instruction ? operand_rs2_pv_sub_sc_b :
		is_pv_avg_h_instruction ? operand_rs2_pv_avg_h :
		is_pv_avg_sc_h_instruction ? operand_rs2_pv_avg_sc_h :
		is_pv_avg_b_instruction ? operand_rs2_pv_avg_b :
		is_pv_avg_sc_b_instruction ? operand_rs2_pv_avg_sc_b :
		is_pv_avgu_h_instruction ? operand_rs2_pv_avgu_h :
		is_pv_avgu_sc_h_instruction ? operand_rs2_pv_avgu_sc_h :
		is_pv_avgu_b_instruction ? operand_rs2_pv_avgu_b :
		is_pv_avgu_sc_b_instruction ? operand_rs2_pv_avgu_sc_b :
		is_pv_min_h_instruction ? operand_rs2_pv_min_h :
		is_pv_min_sc_h_instruction ? operand_rs2_pv_min_sc_h :
		is_pv_min_b_instruction ? operand_rs2_pv_min_b :
		is_pv_min_sc_b_instruction ? operand_rs2_pv_min_sc_b :
		is_pv_minu_h_instruction ? operand_rs2_pv_minu_h :
		is_pv_minu_sc_h_instruction ? operand_rs2_pv_minu_sc_h :
		is_pv_minu_b_instruction ? operand_rs2_pv_minu_b :
		is_pv_minu_sc_b_instruction ? operand_rs2_pv_minu_sc_b :
		is_pv_max_h_instruction ? operand_rs2_pv_max_h :
		is_pv_max_sc_h_instruction ? operand_rs2_pv_max_sc_h :
		is_pv_max_b_instruction ? operand_rs2_pv_max_b :
		is_pv_max_sc_b_instruction ? operand_rs2_pv_max_sc_b :
		is_pv_maxu_h_instruction ? operand_rs2_pv_maxu_h :
		is_pv_maxu_sc_h_instruction ? operand_rs2_pv_maxu_sc_h :
		is_pv_maxu_b_instruction ? operand_rs2_pv_maxu_b :
		is_pv_maxu_sc_b_instruction ? operand_rs2_pv_maxu_sc_b :
		is_pv_srl_h_instruction ? operand_rs2_pv_srl_h :
		is_pv_srl_sc_h_instruction ? operand_rs2_pv_srl_sc_h :
		is_pv_srl_b_instruction ? operand_rs2_pv_srl_b :
		is_pv_srl_sc_b_instruction ? operand_rs2_pv_srl_sc_b :
		is_pv_sra_h_instruction ? operand_rs2_pv_sra_h :
		is_pv_sra_sc_h_instruction ? operand_rs2_pv_sra_sc_h :
		is_pv_sra_b_instruction ? operand_rs2_pv_sra_b :
		is_pv_sra_sc_b_instruction ? operand_rs2_pv_sra_sc_b :
		is_pv_sll_h_instruction ? operand_rs2_pv_sll_h :
		is_pv_sll_sc_h_instruction ? operand_rs2_pv_sll_sc_h :
		is_pv_sll_b_instruction ? operand_rs2_pv_sll_b :
		is_pv_sll_sc_b_instruction ? operand_rs2_pv_sll_sc_b :
		is_pv_or_h_instruction ? operand_rs2_pv_or_h :
		is_pv_or_sc_h_instruction ? operand_rs2_pv_or_sc_h :
		is_pv_or_b_instruction ? operand_rs2_pv_or_b :
		is_pv_or_sc_b_instruction ? operand_rs2_pv_or_sc_b :
		is_pv_xor_h_instruction ? operand_rs2_pv_xor_h :
		is_pv_xor_sc_h_instruction ? operand_rs2_pv_xor_sc_h :
		is_pv_xor_b_instruction ? operand_rs2_pv_xor_b :
		is_pv_xor_sc_b_instruction ? operand_rs2_pv_xor_sc_b :
		is_pv_and_h_instruction ? operand_rs2_pv_and_h :
		is_pv_and_sc_h_instruction ? operand_rs2_pv_and_sc_h :
		is_pv_and_b_instruction ? operand_rs2_pv_and_b :
		is_pv_and_sc_b_instruction ? operand_rs2_pv_and_sc_b :
		is_pv_dotup_h_instruction ? operand_rs2_pv_dotup_h :
		is_pv_dotup_sc_h_instruction ? operand_rs2_pv_dotup_sc_h :
		is_pv_dotup_b_instruction ? operand_rs2_pv_dotup_b :
		is_pv_dotup_sc_b_instruction ? operand_rs2_pv_dotup_sc_b :
		is_pv_dotusp_h_instruction ? operand_rs2_pv_dotusp_h :
		is_pv_dotusp_sc_h_instruction ? operand_rs2_pv_dotusp_sc_h :
		is_pv_dotusp_b_instruction ? operand_rs2_pv_dotusp_b :
		is_pv_dotusp_sc_b_instruction ? operand_rs2_pv_dotusp_sc_b :
		is_pv_dotsp_h_instruction ? operand_rs2_pv_dotsp_h :
		is_pv_dotsp_sc_h_instruction ? operand_rs2_pv_dotsp_sc_h :
		is_pv_dotsp_b_instruction ? operand_rs2_pv_dotsp_b :
		is_pv_dotsp_sc_b_instruction ? operand_rs2_pv_dotsp_sc_b :
		is_pv_sdotup_h_instruction ? operand_rs2_pv_sdotup_h :
		is_pv_sdotup_sc_h_instruction ? operand_rs2_pv_sdotup_sc_h :
		is_pv_sdotup_b_instruction ? operand_rs2_pv_sdotup_b :
		is_pv_sdotup_sc_b_instruction ? operand_rs2_pv_sdotup_sc_b :
		is_pv_sdotusp_h_instruction ? operand_rs2_pv_sdotusp_h :
		is_pv_sdotusp_sc_h_instruction ? operand_rs2_pv_sdotusp_sc_h :
		is_pv_sdotusp_b_instruction ? operand_rs2_pv_sdotusp_b :
		is_pv_sdotusp_sc_b_instruction ? operand_rs2_pv_sdotusp_sc_b :
		is_pv_sdotsp_h_instruction ? operand_rs2_pv_sdotsp_h :
		is_pv_sdotsp_sc_h_instruction ? operand_rs2_pv_sdotsp_sc_h :
		is_pv_sdotsp_b_instruction ? operand_rs2_pv_sdotsp_b :
		is_pv_sdotsp_sc_b_instruction ? operand_rs2_pv_sdotsp_sc_b :
		is_pv_shuffle_h_instruction ? operand_rs2_pv_shuffle_h :
		is_pv_shuffle_b_instruction ? operand_rs2_pv_shuffle_b :
		is_pv_shuffle2_h_instruction ? operand_rs2_pv_shuffle2_h :
		is_pv_shuffle2_b_instruction ? operand_rs2_pv_shuffle2_b :
		is_pv_pack_h_instruction ? operand_rs2_pv_pack_h :
		is_pv_packhi_b_instruction ? operand_rs2_pv_packhi_b :
		is_pv_packlo_b_instruction ? operand_rs2_pv_packlo_b :
		5'h 00;
	assign operand_csr =
		is_csrrw_instruction ? operand_csr_csrrw :
		is_csrrs_instruction ? operand_csr_csrrs :
		is_csrrc_instruction ? operand_csr_csrrc :
		is_csrrwi_instruction ? operand_csr_csrrwi :
		is_csrrsi_instruction ? operand_csr_csrrsi :
		is_csrrci_instruction ? operand_csr_csrrci :
		12'h 00;

endmodule
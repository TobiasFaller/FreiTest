
module rv32imc_zicsr_lui (
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

module rv32imc_zicsr_auipc (
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

module rv32imc_zicsr_jal (
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

module rv32imc_zicsr_jalr (
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

module rv32imc_zicsr_beq (
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

module rv32imc_zicsr_bne (
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

module rv32imc_zicsr_blt (
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

module rv32imc_zicsr_bge (
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

module rv32imc_zicsr_bltu (
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

module rv32imc_zicsr_bgeu (
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

module rv32imc_zicsr_lb (
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

module rv32imc_zicsr_lh (
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

module rv32imc_zicsr_lw (
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

module rv32imc_zicsr_lbu (
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

module rv32imc_zicsr_lhu (
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

module rv32imc_zicsr_sb (
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

module rv32imc_zicsr_sh (
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

module rv32imc_zicsr_sw (
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

module rv32imc_zicsr_addi (
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

module rv32imc_zicsr_slti (
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

module rv32imc_zicsr_sltiu (
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

module rv32imc_zicsr_xori (
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

module rv32imc_zicsr_ori (
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

module rv32imc_zicsr_andi (
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

module rv32imc_zicsr_slli (
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

module rv32imc_zicsr_srli (
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

module rv32imc_zicsr_srai (
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

module rv32imc_zicsr_add (
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

module rv32imc_zicsr_sub (
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

module rv32imc_zicsr_sll (
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

module rv32imc_zicsr_slt (
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

module rv32imc_zicsr_sltu (
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

module rv32imc_zicsr_xor (
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

module rv32imc_zicsr_srl (
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

module rv32imc_zicsr_sra (
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

module rv32imc_zicsr_or (
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

module rv32imc_zicsr_and (
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

module rv32imc_zicsr_fence (
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

module rv32imc_zicsr_ecall (
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

module rv32imc_zicsr_ebreak (
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

module rv32imc_zicsr_mul (
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

module rv32imc_zicsr_mulh (
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

module rv32imc_zicsr_mulhsu (
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

module rv32imc_zicsr_mulhu (
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

module rv32imc_zicsr_div (
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

module rv32imc_zicsr_divu (
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

module rv32imc_zicsr_rem (
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

module rv32imc_zicsr_remu (
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

module rv32imc_zicsr_caddi4spn (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [9:2] operand_imm,
	output wire [2:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 000)
		&& (opcode[17:16] == 2'b 00);

	// Assign the decoded operands
	assign operand_imm[5:4] = opcode[28:27];
	assign operand_imm[9:6] = opcode[26:23];
	assign operand_imm[2:2] = opcode[22:22];
	assign operand_imm[3:3] = opcode[21:21];
	assign operand_rd[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_clw (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [6:2] operand_uimm,
	output wire [2:0] operand_rs1,
	output wire [2:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 010)
		&& (opcode[17:16] == 2'b 00);

	// Assign the decoded operands
	assign operand_uimm[5:3] = opcode[28:26];
	assign operand_uimm[2:2] = opcode[22:22];
	assign operand_uimm[6:6] = opcode[21:21];
	assign operand_rs1[2:0] = opcode[25:23];
	assign operand_rd[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_csw (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [6:2] operand_uimm,
	output wire [2:0] operand_rs1,
	output wire [2:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 110)
		&& (opcode[17:16] == 2'b 00);

	// Assign the decoded operands
	assign operand_uimm[5:3] = opcode[28:26];
	assign operand_uimm[2:2] = opcode[22:22];
	assign operand_uimm[6:6] = opcode[21:21];
	assign operand_rs1[2:0] = opcode[25:23];
	assign operand_rs2[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_caddi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 000)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[5:5] = opcode[28:28];
	assign operand_imm[4:0] = opcode[22:18];
	assign operand_rs1[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_cnop (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_nzimm
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 000)
		&& (opcode[27:23] == 5'b 00000)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_nzimm[5:5] = opcode[28:28];
	assign operand_nzimm[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_cjal (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:1] operand_imm
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 001)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[11:11] = opcode[28:28];
	assign operand_imm[4:4] = opcode[27:27];
	assign operand_imm[9:8] = opcode[26:25];
	assign operand_imm[10:10] = opcode[24:24];
	assign operand_imm[6:6] = opcode[23:23];
	assign operand_imm[7:7] = opcode[22:22];
	assign operand_imm[3:1] = opcode[21:19];
	assign operand_imm[5:5] = opcode[18:18];

endmodule

module rv32imc_zicsr_cli (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 010)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[5:5] = opcode[28:28];
	assign operand_imm[4:0] = opcode[22:18];
	assign operand_rd[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_clui (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [17:12] operand_imm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 011)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[17:17] = opcode[28:28];
	assign operand_imm[16:12] = opcode[22:18];
	assign operand_rd[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_caddi16sp (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [9:4] operand_nzimm
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 011)
		&& (opcode[27:23] == 5'b 00010)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_nzimm[9:9] = opcode[28:28];
	assign operand_nzimm[4:4] = opcode[22:22];
	assign operand_nzimm[6:6] = opcode[21:21];
	assign operand_nzimm[8:7] = opcode[20:19];
	assign operand_nzimm[5:5] = opcode[18:18];

endmodule

module rv32imc_zicsr_csrli (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rs1,
	output wire [4:0] operand_shamt
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 00)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rs1[2:0] = opcode[25:23];
	assign operand_shamt[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_csrai (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rs1,
	output wire [4:0] operand_shamt
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 01)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rs1[2:0] = opcode[25:23];
	assign operand_shamt[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_candi (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [5:0] operand_imm,
	output wire [2:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[27:26] == 2'b 10)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[5:5] = opcode[28:28];
	assign operand_imm[4:0] = opcode[22:18];
	assign operand_rs1[2:0] = opcode[25:23];

endmodule

module rv32imc_zicsr_csub (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rd,
	output wire [2:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 11)
		&& (opcode[22:21] == 2'b 00)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rd[2:0] = opcode[25:23];
	assign operand_rs2[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_cxor (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rd,
	output wire [2:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 11)
		&& (opcode[22:21] == 2'b 01)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rd[2:0] = opcode[25:23];
	assign operand_rs2[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_cor (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rd,
	output wire [2:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 11)
		&& (opcode[22:21] == 2'b 10)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rd[2:0] = opcode[25:23];
	assign operand_rs2[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_cand (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [2:0] operand_rd,
	output wire [2:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[27:26] == 2'b 11)
		&& (opcode[22:21] == 2'b 11)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_rd[2:0] = opcode[25:23];
	assign operand_rs2[2:0] = opcode[20:18];

endmodule

module rv32imc_zicsr_cj (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [11:1] operand_imm
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 101)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[11:11] = opcode[28:28];
	assign operand_imm[4:4] = opcode[27:27];
	assign operand_imm[9:8] = opcode[26:25];
	assign operand_imm[10:10] = opcode[24:24];
	assign operand_imm[6:6] = opcode[23:23];
	assign operand_imm[7:7] = opcode[22:22];
	assign operand_imm[3:1] = opcode[21:19];
	assign operand_imm[5:5] = opcode[18:18];

endmodule

module rv32imc_zicsr_cbeqz (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [8:1] operand_imm,
	output wire [2:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 110)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[8:8] = opcode[28:28];
	assign operand_imm[4:3] = opcode[27:26];
	assign operand_imm[7:6] = opcode[22:21];
	assign operand_imm[2:1] = opcode[20:19];
	assign operand_imm[5:5] = opcode[18:18];
	assign operand_rs1[2:0] = opcode[25:23];

endmodule

module rv32imc_zicsr_cbnez (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [8:1] operand_imm,
	output wire [2:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 111)
		&& (opcode[17:16] == 2'b 01);

	// Assign the decoded operands
	assign operand_imm[8:8] = opcode[28:28];
	assign operand_imm[4:3] = opcode[27:26];
	assign operand_imm[7:6] = opcode[22:21];
	assign operand_imm[2:1] = opcode[20:19];
	assign operand_imm[5:5] = opcode[18:18];
	assign operand_rs1[2:0] = opcode[25:23];

endmodule

module rv32imc_zicsr_cslli (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_nzuimm
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 000)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[27:23];
	assign operand_nzuimm[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_clwsp (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [7:2] operand_uimm,
	output wire [4:0] operand_rd
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 010)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_uimm[5:5] = opcode[28:28];
	assign operand_uimm[4:2] = opcode[22:20];
	assign operand_uimm[7:6] = opcode[19:18];
	assign operand_rd[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_cmv (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rd,
	output wire [4:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_rd[4:0] = opcode[27:23];
	assign operand_rs2[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_cjr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 0)
		&& (opcode[22:18] == 5'b 00000)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_cadd (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rd,
	output wire [4:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 1)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_rd[4:0] = opcode[27:23];
	assign operand_rs2[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_cjalr (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [4:0] operand_rs1
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 1)
		&& (opcode[22:18] == 5'b 00000)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_rs1[4:0] = opcode[27:23];

endmodule

module rv32imc_zicsr_cebreak (
	input wire [31:0] opcode,
	output wire opcode_valid,
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 100)
		&& (opcode[28:28] == 1'b 1)
		&& (opcode[27:23] == 5'b 00000)
		&& (opcode[22:18] == 5'b 00000)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands

endmodule

module rv32imc_zicsr_cswsp (
	input wire [31:0] opcode,
	output wire opcode_valid,
	output wire [7:2] operand_uimm,
	output wire [4:0] operand_rs2
);

	// Create the opcode_valid output signal
	assign opcode_valid = 1
		&& (opcode[31:29] == 3'b 110)
		&& (opcode[17:16] == 2'b 10);

	// Assign the decoded operands
	assign operand_uimm[5:2] = opcode[28:25];
	assign operand_uimm[7:6] = opcode[24:23];
	assign operand_rs2[4:0] = opcode[22:18];

endmodule

module rv32imc_zicsr_csrrw (
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

module rv32imc_zicsr_csrrs (
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

module rv32imc_zicsr_csrrc (
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

module rv32imc_zicsr_csrrwi (
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

module rv32imc_zicsr_csrrsi (
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

module rv32imc_zicsr_csrrci (
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


module rv32imc_zicsr (
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
	output wire is_caddi4spn_instruction,
	output wire is_clw_instruction,
	output wire is_csw_instruction,
	output wire is_caddi_instruction,
	output wire is_cnop_instruction,
	output wire is_cjal_instruction,
	output wire is_cli_instruction,
	output wire is_clui_instruction,
	output wire is_caddi16sp_instruction,
	output wire is_csrli_instruction,
	output wire is_csrai_instruction,
	output wire is_candi_instruction,
	output wire is_csub_instruction,
	output wire is_cxor_instruction,
	output wire is_cor_instruction,
	output wire is_cand_instruction,
	output wire is_cj_instruction,
	output wire is_cbeqz_instruction,
	output wire is_cbnez_instruction,
	output wire is_cslli_instruction,
	output wire is_clwsp_instruction,
	output wire is_cmv_instruction,
	output wire is_cjr_instruction,
	output wire is_cadd_instruction,
	output wire is_cjalr_instruction,
	output wire is_cebreak_instruction,
	output wire is_cswsp_instruction,
	output wire is_csrrw_instruction,
	output wire is_csrrs_instruction,
	output wire is_csrrc_instruction,
	output wire is_csrrwi_instruction,
	output wire is_csrrsi_instruction,
	output wire is_csrrci_instruction,
	output wire has_operand_csr,
	output wire has_operand_fm,
	output wire has_operand_imm,
	output wire has_operand_nzimm,
	output wire has_operand_nzuimm,
	output wire has_operand_pred,
	output wire has_operand_rd,
	output wire has_operand_rs1,
	output wire has_operand_rs2,
	output wire has_operand_shamt,
	output wire has_operand_succ,
	output wire has_operand_uimm,
	output wire has_operand_zimm,
	output wire [4:0] operand_rd,
	output wire [4:0] operand_rs1,
	output wire [4:0] operand_rs2,
	output wire [11:0] operand_csr,
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
	output wire [9:2] operand_imm_caddi4spn,
	output wire [5:0] operand_imm_caddi,
	output wire [11:1] operand_imm_cjal,
	output wire [5:0] operand_imm_cli,
	output wire [17:12] operand_imm_clui,
	output wire [5:0] operand_imm_candi,
	output wire [11:1] operand_imm_cj,
	output wire [8:1] operand_imm_cbeqz,
	output wire [8:1] operand_imm_cbnez,
	output wire [5:0] operand_nzimm_cnop,
	output wire [9:4] operand_nzimm_caddi16sp,
	output wire [4:0] operand_nzuimm_cslli,
	output wire [3:0] operand_pred_fence,
	output wire [4:0] operand_shamt_slli,
	output wire [4:0] operand_shamt_srli,
	output wire [4:0] operand_shamt_srai,
	output wire [4:0] operand_shamt_csrli,
	output wire [4:0] operand_shamt_csrai,
	output wire [3:0] operand_succ_fence,
	output wire [6:2] operand_uimm_clw,
	output wire [6:2] operand_uimm_csw,
	output wire [7:2] operand_uimm_clwsp,
	output wire [7:2] operand_uimm_cswsp,
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
	wire [2:0] operand_rd_caddi4spn;
	wire [2:0] operand_rd_clw;
	wire [4:0] operand_rd_cli;
	wire [4:0] operand_rd_clui;
	wire [2:0] operand_rd_csub;
	wire [2:0] operand_rd_cxor;
	wire [2:0] operand_rd_cor;
	wire [2:0] operand_rd_cand;
	wire [4:0] operand_rd_clwsp;
	wire [4:0] operand_rd_cmv;
	wire [4:0] operand_rd_cadd;
	wire [4:0] operand_rd_csrrw;
	wire [4:0] operand_rd_csrrs;
	wire [4:0] operand_rd_csrrc;
	wire [4:0] operand_rd_csrrwi;
	wire [4:0] operand_rd_csrrsi;
	wire [4:0] operand_rd_csrrci;
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
	wire [2:0] operand_rs1_clw;
	wire [2:0] operand_rs1_csw;
	wire [4:0] operand_rs1_caddi;
	wire [2:0] operand_rs1_csrli;
	wire [2:0] operand_rs1_csrai;
	wire [2:0] operand_rs1_candi;
	wire [2:0] operand_rs1_cbeqz;
	wire [2:0] operand_rs1_cbnez;
	wire [4:0] operand_rs1_cslli;
	wire [4:0] operand_rs1_cjr;
	wire [4:0] operand_rs1_cjalr;
	wire [4:0] operand_rs1_csrrw;
	wire [4:0] operand_rs1_csrrs;
	wire [4:0] operand_rs1_csrrc;
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
	wire [2:0] operand_rs2_csw;
	wire [2:0] operand_rs2_csub;
	wire [2:0] operand_rs2_cxor;
	wire [2:0] operand_rs2_cor;
	wire [2:0] operand_rs2_cand;
	wire [4:0] operand_rs2_cmv;
	wire [4:0] operand_rs2_cadd;
	wire [4:0] operand_rs2_cswsp;
	wire [11:0] operand_csr_csrrw;
	wire [11:0] operand_csr_csrrs;
	wire [11:0] operand_csr_csrrc;
	wire [11:0] operand_csr_csrrwi;
	wire [11:0] operand_csr_csrrsi;
	wire [11:0] operand_csr_csrrci;


	// Instance for lui instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32imc_zicsr_lui lui_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lui_instruction)
		, .operand_imm(operand_imm_lui)
		, .operand_rd(operand_rd_lui)
	);

	// Instance for auipc instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32imc_zicsr_auipc auipc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_auipc_instruction)
		, .operand_imm(operand_imm_auipc)
		, .operand_rd(operand_rd_auipc)
	);

	// Instance for jal instruction
	// Operands [imm (20 bits), rd (5 bits)]
	rv32imc_zicsr_jal jal_inst (
		  .opcode(opcode)
		, .opcode_valid(is_jal_instruction)
		, .operand_imm(operand_imm_jal)
		, .operand_rd(operand_rd_jal)
	);

	// Instance for jalr instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_jalr jalr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_jalr_instruction)
		, .operand_imm(operand_imm_jalr)
		, .operand_rs1(operand_rs1_jalr)
		, .operand_rd(operand_rd_jalr)
	);

	// Instance for beq instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_beq beq_inst (
		  .opcode(opcode)
		, .opcode_valid(is_beq_instruction)
		, .operand_imm(operand_imm_beq)
		, .operand_rs2(operand_rs2_beq)
		, .operand_rs1(operand_rs1_beq)
	);

	// Instance for bne instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_bne bne_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bne_instruction)
		, .operand_imm(operand_imm_bne)
		, .operand_rs2(operand_rs2_bne)
		, .operand_rs1(operand_rs1_bne)
	);

	// Instance for blt instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_blt blt_inst (
		  .opcode(opcode)
		, .opcode_valid(is_blt_instruction)
		, .operand_imm(operand_imm_blt)
		, .operand_rs2(operand_rs2_blt)
		, .operand_rs1(operand_rs1_blt)
	);

	// Instance for bge instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_bge bge_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bge_instruction)
		, .operand_imm(operand_imm_bge)
		, .operand_rs2(operand_rs2_bge)
		, .operand_rs1(operand_rs1_bge)
	);

	// Instance for bltu instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_bltu bltu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bltu_instruction)
		, .operand_imm(operand_imm_bltu)
		, .operand_rs2(operand_rs2_bltu)
		, .operand_rs1(operand_rs1_bltu)
	);

	// Instance for bgeu instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_bgeu bgeu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_bgeu_instruction)
		, .operand_imm(operand_imm_bgeu)
		, .operand_rs2(operand_rs2_bgeu)
		, .operand_rs1(operand_rs1_bgeu)
	);

	// Instance for lb instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_lb lb_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lb_instruction)
		, .operand_imm(operand_imm_lb)
		, .operand_rs1(operand_rs1_lb)
		, .operand_rd(operand_rd_lb)
	);

	// Instance for lh instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_lh lh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lh_instruction)
		, .operand_imm(operand_imm_lh)
		, .operand_rs1(operand_rs1_lh)
		, .operand_rd(operand_rd_lh)
	);

	// Instance for lw instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_lw lw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lw_instruction)
		, .operand_imm(operand_imm_lw)
		, .operand_rs1(operand_rs1_lw)
		, .operand_rd(operand_rd_lw)
	);

	// Instance for lbu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_lbu lbu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lbu_instruction)
		, .operand_imm(operand_imm_lbu)
		, .operand_rs1(operand_rs1_lbu)
		, .operand_rd(operand_rd_lbu)
	);

	// Instance for lhu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_lhu lhu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_lhu_instruction)
		, .operand_imm(operand_imm_lhu)
		, .operand_rs1(operand_rs1_lhu)
		, .operand_rd(operand_rd_lhu)
	);

	// Instance for sb instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_sb sb_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sb_instruction)
		, .operand_imm(operand_imm_sb)
		, .operand_rs2(operand_rs2_sb)
		, .operand_rs1(operand_rs1_sb)
	);

	// Instance for sh instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_sh sh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sh_instruction)
		, .operand_imm(operand_imm_sh)
		, .operand_rs2(operand_rs2_sh)
		, .operand_rs1(operand_rs1_sh)
	);

	// Instance for sw instruction
	// Operands [imm (12 bits), rs2 (5 bits), rs1 (5 bits)]
	rv32imc_zicsr_sw sw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sw_instruction)
		, .operand_imm(operand_imm_sw)
		, .operand_rs2(operand_rs2_sw)
		, .operand_rs1(operand_rs1_sw)
	);

	// Instance for addi instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_addi addi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_addi_instruction)
		, .operand_imm(operand_imm_addi)
		, .operand_rs1(operand_rs1_addi)
		, .operand_rd(operand_rd_addi)
	);

	// Instance for slti instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_slti slti_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slti_instruction)
		, .operand_imm(operand_imm_slti)
		, .operand_rs1(operand_rs1_slti)
		, .operand_rd(operand_rd_slti)
	);

	// Instance for sltiu instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_sltiu sltiu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sltiu_instruction)
		, .operand_imm(operand_imm_sltiu)
		, .operand_rs1(operand_rs1_sltiu)
		, .operand_rd(operand_rd_sltiu)
	);

	// Instance for xori instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_xori xori_inst (
		  .opcode(opcode)
		, .opcode_valid(is_xori_instruction)
		, .operand_imm(operand_imm_xori)
		, .operand_rs1(operand_rs1_xori)
		, .operand_rd(operand_rd_xori)
	);

	// Instance for ori instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_ori ori_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ori_instruction)
		, .operand_imm(operand_imm_ori)
		, .operand_rs1(operand_rs1_ori)
		, .operand_rd(operand_rd_ori)
	);

	// Instance for andi instruction
	// Operands [imm (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_andi andi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_andi_instruction)
		, .operand_imm(operand_imm_andi)
		, .operand_rs1(operand_rs1_andi)
		, .operand_rd(operand_rd_andi)
	);

	// Instance for slli instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_slli slli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slli_instruction)
		, .operand_shamt(operand_shamt_slli)
		, .operand_rs1(operand_rs1_slli)
		, .operand_rd(operand_rd_slli)
	);

	// Instance for srli instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_srli srli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srli_instruction)
		, .operand_shamt(operand_shamt_srli)
		, .operand_rs1(operand_rs1_srli)
		, .operand_rd(operand_rd_srli)
	);

	// Instance for srai instruction
	// Operands [shamt (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_srai srai_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srai_instruction)
		, .operand_shamt(operand_shamt_srai)
		, .operand_rs1(operand_rs1_srai)
		, .operand_rd(operand_rd_srai)
	);

	// Instance for add instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_add add_inst (
		  .opcode(opcode)
		, .opcode_valid(is_add_instruction)
		, .operand_rs2(operand_rs2_add)
		, .operand_rs1(operand_rs1_add)
		, .operand_rd(operand_rd_add)
	);

	// Instance for sub instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_sub sub_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sub_instruction)
		, .operand_rs2(operand_rs2_sub)
		, .operand_rs1(operand_rs1_sub)
		, .operand_rd(operand_rd_sub)
	);

	// Instance for sll instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_sll sll_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sll_instruction)
		, .operand_rs2(operand_rs2_sll)
		, .operand_rs1(operand_rs1_sll)
		, .operand_rd(operand_rd_sll)
	);

	// Instance for slt instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_slt slt_inst (
		  .opcode(opcode)
		, .opcode_valid(is_slt_instruction)
		, .operand_rs2(operand_rs2_slt)
		, .operand_rs1(operand_rs1_slt)
		, .operand_rd(operand_rd_slt)
	);

	// Instance for sltu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_sltu sltu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sltu_instruction)
		, .operand_rs2(operand_rs2_sltu)
		, .operand_rs1(operand_rs1_sltu)
		, .operand_rd(operand_rd_sltu)
	);

	// Instance for xor instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_xor xor_inst (
		  .opcode(opcode)
		, .opcode_valid(is_xor_instruction)
		, .operand_rs2(operand_rs2_xor)
		, .operand_rs1(operand_rs1_xor)
		, .operand_rd(operand_rd_xor)
	);

	// Instance for srl instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_srl srl_inst (
		  .opcode(opcode)
		, .opcode_valid(is_srl_instruction)
		, .operand_rs2(operand_rs2_srl)
		, .operand_rs1(operand_rs1_srl)
		, .operand_rd(operand_rd_srl)
	);

	// Instance for sra instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_sra sra_inst (
		  .opcode(opcode)
		, .opcode_valid(is_sra_instruction)
		, .operand_rs2(operand_rs2_sra)
		, .operand_rs1(operand_rs1_sra)
		, .operand_rd(operand_rd_sra)
	);

	// Instance for or instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_or or_inst (
		  .opcode(opcode)
		, .opcode_valid(is_or_instruction)
		, .operand_rs2(operand_rs2_or)
		, .operand_rs1(operand_rs1_or)
		, .operand_rd(operand_rd_or)
	);

	// Instance for and instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_and and_inst (
		  .opcode(opcode)
		, .opcode_valid(is_and_instruction)
		, .operand_rs2(operand_rs2_and)
		, .operand_rs1(operand_rs1_and)
		, .operand_rd(operand_rd_and)
	);

	// Instance for fence instruction
	// Operands [fm (4 bits), pred (4 bits), succ (4 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_fence fence_inst (
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
	rv32imc_zicsr_ecall ecall_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ecall_instruction)
	);

	// Instance for ebreak instruction
	// Operands []
	rv32imc_zicsr_ebreak ebreak_inst (
		  .opcode(opcode)
		, .opcode_valid(is_ebreak_instruction)
	);

	// Instance for mul instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_mul mul_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mul_instruction)
		, .operand_rs2(operand_rs2_mul)
		, .operand_rs1(operand_rs1_mul)
		, .operand_rd(operand_rd_mul)
	);

	// Instance for mulh instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_mulh mulh_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulh_instruction)
		, .operand_rs2(operand_rs2_mulh)
		, .operand_rs1(operand_rs1_mulh)
		, .operand_rd(operand_rd_mulh)
	);

	// Instance for mulhsu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_mulhsu mulhsu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulhsu_instruction)
		, .operand_rs2(operand_rs2_mulhsu)
		, .operand_rs1(operand_rs1_mulhsu)
		, .operand_rd(operand_rd_mulhsu)
	);

	// Instance for mulhu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_mulhu mulhu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_mulhu_instruction)
		, .operand_rs2(operand_rs2_mulhu)
		, .operand_rs1(operand_rs1_mulhu)
		, .operand_rd(operand_rd_mulhu)
	);

	// Instance for div instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_div div_inst (
		  .opcode(opcode)
		, .opcode_valid(is_div_instruction)
		, .operand_rs2(operand_rs2_div)
		, .operand_rs1(operand_rs1_div)
		, .operand_rd(operand_rd_div)
	);

	// Instance for divu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_divu divu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_divu_instruction)
		, .operand_rs2(operand_rs2_divu)
		, .operand_rs1(operand_rs1_divu)
		, .operand_rd(operand_rd_divu)
	);

	// Instance for rem instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_rem rem_inst (
		  .opcode(opcode)
		, .opcode_valid(is_rem_instruction)
		, .operand_rs2(operand_rs2_rem)
		, .operand_rs1(operand_rs1_rem)
		, .operand_rd(operand_rd_rem)
	);

	// Instance for remu instruction
	// Operands [rs2 (5 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_remu remu_inst (
		  .opcode(opcode)
		, .opcode_valid(is_remu_instruction)
		, .operand_rs2(operand_rs2_remu)
		, .operand_rs1(operand_rs1_remu)
		, .operand_rd(operand_rd_remu)
	);

	// Instance for caddi4spn instruction
	// Operands [imm (8 bits), rd (3 bits)]
	rv32imc_zicsr_caddi4spn caddi4spn_inst (
		  .opcode(opcode)
		, .opcode_valid(is_caddi4spn_instruction)
		, .operand_imm(operand_imm_caddi4spn)
		, .operand_rd(operand_rd_caddi4spn)
	);

	// Instance for clw instruction
	// Operands [uimm (5 bits), rs1 (3 bits), rd (3 bits)]
	rv32imc_zicsr_clw clw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_clw_instruction)
		, .operand_uimm(operand_uimm_clw)
		, .operand_rs1(operand_rs1_clw)
		, .operand_rd(operand_rd_clw)
	);

	// Instance for csw instruction
	// Operands [uimm (5 bits), rs1 (3 bits), rs2 (3 bits)]
	rv32imc_zicsr_csw csw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csw_instruction)
		, .operand_uimm(operand_uimm_csw)
		, .operand_rs1(operand_rs1_csw)
		, .operand_rs2(operand_rs2_csw)
	);

	// Instance for caddi instruction
	// Operands [imm (6 bits), rs1 (5 bits)]
	rv32imc_zicsr_caddi caddi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_caddi_instruction)
		, .operand_imm(operand_imm_caddi)
		, .operand_rs1(operand_rs1_caddi)
	);

	// Instance for cnop instruction
	// Operands [nzimm (6 bits)]
	rv32imc_zicsr_cnop cnop_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cnop_instruction)
		, .operand_nzimm(operand_nzimm_cnop)
	);

	// Instance for cjal instruction
	// Operands [imm (11 bits)]
	rv32imc_zicsr_cjal cjal_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cjal_instruction)
		, .operand_imm(operand_imm_cjal)
	);

	// Instance for cli instruction
	// Operands [imm (6 bits), rd (5 bits)]
	rv32imc_zicsr_cli cli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cli_instruction)
		, .operand_imm(operand_imm_cli)
		, .operand_rd(operand_rd_cli)
	);

	// Instance for clui instruction
	// Operands [imm (6 bits), rd (5 bits)]
	rv32imc_zicsr_clui clui_inst (
		  .opcode(opcode)
		, .opcode_valid(is_clui_instruction)
		, .operand_imm(operand_imm_clui)
		, .operand_rd(operand_rd_clui)
	);

	// Instance for caddi16sp instruction
	// Operands [nzimm (6 bits)]
	rv32imc_zicsr_caddi16sp caddi16sp_inst (
		  .opcode(opcode)
		, .opcode_valid(is_caddi16sp_instruction)
		, .operand_nzimm(operand_nzimm_caddi16sp)
	);

	// Instance for csrli instruction
	// Operands [rs1 (3 bits), shamt (5 bits)]
	rv32imc_zicsr_csrli csrli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrli_instruction)
		, .operand_rs1(operand_rs1_csrli)
		, .operand_shamt(operand_shamt_csrli)
	);

	// Instance for csrai instruction
	// Operands [rs1 (3 bits), shamt (5 bits)]
	rv32imc_zicsr_csrai csrai_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrai_instruction)
		, .operand_rs1(operand_rs1_csrai)
		, .operand_shamt(operand_shamt_csrai)
	);

	// Instance for candi instruction
	// Operands [imm (6 bits), rs1 (3 bits)]
	rv32imc_zicsr_candi candi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_candi_instruction)
		, .operand_imm(operand_imm_candi)
		, .operand_rs1(operand_rs1_candi)
	);

	// Instance for csub instruction
	// Operands [rd (3 bits), rs2 (3 bits)]
	rv32imc_zicsr_csub csub_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csub_instruction)
		, .operand_rd(operand_rd_csub)
		, .operand_rs2(operand_rs2_csub)
	);

	// Instance for cxor instruction
	// Operands [rd (3 bits), rs2 (3 bits)]
	rv32imc_zicsr_cxor cxor_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cxor_instruction)
		, .operand_rd(operand_rd_cxor)
		, .operand_rs2(operand_rs2_cxor)
	);

	// Instance for cor instruction
	// Operands [rd (3 bits), rs2 (3 bits)]
	rv32imc_zicsr_cor cor_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cor_instruction)
		, .operand_rd(operand_rd_cor)
		, .operand_rs2(operand_rs2_cor)
	);

	// Instance for cand instruction
	// Operands [rd (3 bits), rs2 (3 bits)]
	rv32imc_zicsr_cand cand_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cand_instruction)
		, .operand_rd(operand_rd_cand)
		, .operand_rs2(operand_rs2_cand)
	);

	// Instance for cj instruction
	// Operands [imm (11 bits)]
	rv32imc_zicsr_cj cj_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cj_instruction)
		, .operand_imm(operand_imm_cj)
	);

	// Instance for cbeqz instruction
	// Operands [imm (8 bits), rs1 (3 bits)]
	rv32imc_zicsr_cbeqz cbeqz_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cbeqz_instruction)
		, .operand_imm(operand_imm_cbeqz)
		, .operand_rs1(operand_rs1_cbeqz)
	);

	// Instance for cbnez instruction
	// Operands [imm (8 bits), rs1 (3 bits)]
	rv32imc_zicsr_cbnez cbnez_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cbnez_instruction)
		, .operand_imm(operand_imm_cbnez)
		, .operand_rs1(operand_rs1_cbnez)
	);

	// Instance for cslli instruction
	// Operands [rs1 (5 bits), nzuimm (5 bits)]
	rv32imc_zicsr_cslli cslli_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cslli_instruction)
		, .operand_rs1(operand_rs1_cslli)
		, .operand_nzuimm(operand_nzuimm_cslli)
	);

	// Instance for clwsp instruction
	// Operands [uimm (6 bits), rd (5 bits)]
	rv32imc_zicsr_clwsp clwsp_inst (
		  .opcode(opcode)
		, .opcode_valid(is_clwsp_instruction)
		, .operand_uimm(operand_uimm_clwsp)
		, .operand_rd(operand_rd_clwsp)
	);

	// Instance for cmv instruction
	// Operands [rd (5 bits), rs2 (5 bits)]
	rv32imc_zicsr_cmv cmv_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cmv_instruction)
		, .operand_rd(operand_rd_cmv)
		, .operand_rs2(operand_rs2_cmv)
	);

	// Instance for cjr instruction
	// Operands [rs1 (5 bits)]
	rv32imc_zicsr_cjr cjr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cjr_instruction)
		, .operand_rs1(operand_rs1_cjr)
	);

	// Instance for cadd instruction
	// Operands [rd (5 bits), rs2 (5 bits)]
	rv32imc_zicsr_cadd cadd_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cadd_instruction)
		, .operand_rd(operand_rd_cadd)
		, .operand_rs2(operand_rs2_cadd)
	);

	// Instance for cjalr instruction
	// Operands [rs1 (5 bits)]
	rv32imc_zicsr_cjalr cjalr_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cjalr_instruction)
		, .operand_rs1(operand_rs1_cjalr)
	);

	// Instance for cebreak instruction
	// Operands []
	rv32imc_zicsr_cebreak cebreak_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cebreak_instruction)
	);

	// Instance for cswsp instruction
	// Operands [uimm (6 bits), rs2 (5 bits)]
	rv32imc_zicsr_cswsp cswsp_inst (
		  .opcode(opcode)
		, .opcode_valid(is_cswsp_instruction)
		, .operand_uimm(operand_uimm_cswsp)
		, .operand_rs2(operand_rs2_cswsp)
	);

	// Instance for csrrw instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrw csrrw_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrw_instruction)
		, .operand_csr(operand_csr_csrrw)
		, .operand_rs1(operand_rs1_csrrw)
		, .operand_rd(operand_rd_csrrw)
	);

	// Instance for csrrs instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrs csrrs_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrs_instruction)
		, .operand_csr(operand_csr_csrrs)
		, .operand_rs1(operand_rs1_csrrs)
		, .operand_rd(operand_rd_csrrs)
	);

	// Instance for csrrc instruction
	// Operands [csr (12 bits), rs1 (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrc csrrc_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrc_instruction)
		, .operand_csr(operand_csr_csrrc)
		, .operand_rs1(operand_rs1_csrrc)
		, .operand_rd(operand_rd_csrrc)
	);

	// Instance for csrrwi instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrwi csrrwi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrwi_instruction)
		, .operand_csr(operand_csr_csrrwi)
		, .operand_zimm(operand_zimm_csrrwi)
		, .operand_rd(operand_rd_csrrwi)
	);

	// Instance for csrrsi instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrsi csrrsi_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrsi_instruction)
		, .operand_csr(operand_csr_csrrsi)
		, .operand_zimm(operand_zimm_csrrsi)
		, .operand_rd(operand_rd_csrrsi)
	);

	// Instance for csrrci instruction
	// Operands [csr (12 bits), zimm (5 bits), rd (5 bits)]
	rv32imc_zicsr_csrrci csrrci_inst (
		  .opcode(opcode)
		, .opcode_valid(is_csrrci_instruction)
		, .operand_csr(operand_csr_csrrci)
		, .operand_zimm(operand_zimm_csrrci)
		, .operand_rd(operand_rd_csrrci)
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
		|| is_caddi4spn_instruction
		|| is_clw_instruction
		|| is_csw_instruction
		|| is_caddi_instruction
		|| is_cnop_instruction
		|| is_cjal_instruction
		|| is_cli_instruction
		|| is_clui_instruction
		|| is_caddi16sp_instruction
		|| is_csrli_instruction
		|| is_csrai_instruction
		|| is_candi_instruction
		|| is_csub_instruction
		|| is_cxor_instruction
		|| is_cor_instruction
		|| is_cand_instruction
		|| is_cj_instruction
		|| is_cbeqz_instruction
		|| is_cbnez_instruction
		|| is_cslli_instruction
		|| is_clwsp_instruction
		|| is_cmv_instruction
		|| is_cjr_instruction
		|| is_cadd_instruction
		|| is_cjalr_instruction
		|| is_cebreak_instruction
		|| is_cswsp_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction;
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
		|| is_caddi4spn_instruction
		|| is_caddi_instruction
		|| is_cjal_instruction
		|| is_cli_instruction
		|| is_clui_instruction
		|| is_candi_instruction
		|| is_cj_instruction
		|| is_cbeqz_instruction
		|| is_cbnez_instruction;
	assign has_operand_nzimm = 0
		|| is_cnop_instruction
		|| is_caddi16sp_instruction;
	assign has_operand_nzuimm = 0
		|| is_cslli_instruction;
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
		|| is_caddi4spn_instruction
		|| is_clw_instruction
		|| is_cli_instruction
		|| is_clui_instruction
		|| is_csub_instruction
		|| is_cxor_instruction
		|| is_cor_instruction
		|| is_cand_instruction
		|| is_clwsp_instruction
		|| is_cmv_instruction
		|| is_cadd_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction
		|| is_csrrwi_instruction
		|| is_csrrsi_instruction
		|| is_csrrci_instruction;
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
		|| is_clw_instruction
		|| is_csw_instruction
		|| is_caddi_instruction
		|| is_csrli_instruction
		|| is_csrai_instruction
		|| is_candi_instruction
		|| is_cbeqz_instruction
		|| is_cbnez_instruction
		|| is_cslli_instruction
		|| is_cjr_instruction
		|| is_cjalr_instruction
		|| is_csrrw_instruction
		|| is_csrrs_instruction
		|| is_csrrc_instruction;
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
		|| is_csw_instruction
		|| is_csub_instruction
		|| is_cxor_instruction
		|| is_cor_instruction
		|| is_cand_instruction
		|| is_cmv_instruction
		|| is_cadd_instruction
		|| is_cswsp_instruction;
	assign has_operand_shamt = 0
		|| is_slli_instruction
		|| is_srli_instruction
		|| is_srai_instruction
		|| is_csrli_instruction
		|| is_csrai_instruction;
	assign has_operand_succ = 0
		|| is_fence_instruction;
	assign has_operand_uimm = 0
		|| is_clw_instruction
		|| is_csw_instruction
		|| is_clwsp_instruction
		|| is_cswsp_instruction;
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
		is_caddi4spn_instruction ? operand_rd_caddi4spn :
		is_clw_instruction ? operand_rd_clw :
		is_cli_instruction ? operand_rd_cli :
		is_clui_instruction ? operand_rd_clui :
		is_csub_instruction ? operand_rd_csub :
		is_cxor_instruction ? operand_rd_cxor :
		is_cor_instruction ? operand_rd_cor :
		is_cand_instruction ? operand_rd_cand :
		is_clwsp_instruction ? operand_rd_clwsp :
		is_cmv_instruction ? operand_rd_cmv :
		is_cadd_instruction ? operand_rd_cadd :
		is_csrrw_instruction ? operand_rd_csrrw :
		is_csrrs_instruction ? operand_rd_csrrs :
		is_csrrc_instruction ? operand_rd_csrrc :
		is_csrrwi_instruction ? operand_rd_csrrwi :
		is_csrrsi_instruction ? operand_rd_csrrsi :
		is_csrrci_instruction ? operand_rd_csrrci :
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
		is_clw_instruction ? operand_rs1_clw :
		is_csw_instruction ? operand_rs1_csw :
		is_caddi_instruction ? operand_rs1_caddi :
		is_csrli_instruction ? operand_rs1_csrli :
		is_csrai_instruction ? operand_rs1_csrai :
		is_candi_instruction ? operand_rs1_candi :
		is_cbeqz_instruction ? operand_rs1_cbeqz :
		is_cbnez_instruction ? operand_rs1_cbnez :
		is_cslli_instruction ? operand_rs1_cslli :
		is_cjr_instruction ? operand_rs1_cjr :
		is_cjalr_instruction ? operand_rs1_cjalr :
		is_csrrw_instruction ? operand_rs1_csrrw :
		is_csrrs_instruction ? operand_rs1_csrrs :
		is_csrrc_instruction ? operand_rs1_csrrc :
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
		is_csw_instruction ? operand_rs2_csw :
		is_csub_instruction ? operand_rs2_csub :
		is_cxor_instruction ? operand_rs2_cxor :
		is_cor_instruction ? operand_rs2_cor :
		is_cand_instruction ? operand_rs2_cand :
		is_cmv_instruction ? operand_rs2_cmv :
		is_cadd_instruction ? operand_rs2_cadd :
		is_cswsp_instruction ? operand_rs2_cswsp :
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
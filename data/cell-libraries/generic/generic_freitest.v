// Copyright (c) 2025 Tobias Faller
// SPDX-License-Identifier: MIT

`celldefine

module BUF(input IN1, output OUT);
	buf gate(OUT, IN1);
endmodule

module INV(input IN1, output OUT);
	not gate(OUT, IN1);
endmodule

module AND(input IN1, input IN2, output OUT);
	and gate(OUT, IN1, IN2);
endmodule

module NAND(input IN1, input IN2, output OUT);
	nand gate(OUT, IN1, IN2);
endmodule

module OR(input IN1, input IN2, output OUT);
	or gate(OUT, IN1, IN2);
endmodule

module NOR(input IN1, input IN2, output OUT);
	nor gate(OUT, IN1, IN2);
endmodule

module XOR(input IN1, input IN2, output OUT);
	xor gate(OUT, IN1, IN2);
endmodule

module XNOR(input IN1, input IN2, output OUT);
	xnor gate(OUT, IN1, IN2);
endmodule

module MUX(input IN1, input IN2, input IN3, output OUT);
	mux gate(OUT, IN1, IN2, IN3);
endmodule

module DFF(input IN1, input CLOCK, output OUT);
	wire set;
	wire reset;
	wire qn;
	tie0 gate_set(set);
	tie0 gate_reset(reset);
	dflipflop gate(.d(IN1), .clock(CLOCK), .set(set), .reset(reset), .q(OUT), .qn(qn));
endmodule

`endcelldefine

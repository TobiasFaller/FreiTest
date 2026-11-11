// Copyright (c) 2025 Tobias Faller
// SPDX-License-Identifier: MIT

`celldefine

module BUF(input IN1, output OUT);
	assign OUT = IN1;
endmodule

module INV(input IN1, output OUT);
	assign OUT = !IN1;
endmodule

module AND(input IN1, input IN2, output OUT);
	assign OUT = IN1 & IN2;
endmodule

module OR(input IN1, input IN2, output OUT);
	assign OUT = IN1 | IN2;
endmodule

module XOR(input IN1, input IN2, output OUT);
	assign OUT = IN1 ^ IN2;
endmodule

module NAND(input IN1, input IN2, output OUT);
	assign OUT = !(IN1 & IN2);
endmodule

module NOR(input IN1, input IN2, output OUT);
	assign OUT = !(IN1 | IN2);
endmodule

module XNOR(input IN1, input IN2, output OUT);
	assign OUT = !(IN1 ^ IN2);
endmodule

module MUX(input IN1, input IN2, input IN3, output OUT);
	assign OUT = !IN3 ? IN1 : IN2;
endmodule

module DL(input IN1, input ENABLE, output OUT);
	reg out_reg;
	/* verilator lint_off COMBDLY */
	always_latch @(*) begin
		if (ENABLE) out_reg <= IN1;
	end
	/* verilator lint_on COMBDLY */
	assign OUT = out_reg;
endmodule

module DLR(input IN1, input ENABLE, input ARESET, output OUT);
	/* verilator lint_off UNOPTFLAT */
	reg out_reg;
	/* verilator lint_off COMBDLY */
	always_latch @(*) begin
		if (ARESET) out_reg <= 1'b 0;
		else if (ENABLE) out_reg <= IN1;
	end
	/* verilator lint_on COMBDLY */
	assign OUT = out_reg;
	/* verilator lint_on UNOPTFLAT */
endmodule

module DFF(input IN1, input CLOCK, output OUT);
	reg int_reg;
	reg out_reg;
	always_ff @(posedge CLOCK) int_reg <= IN1;
	always_ff @(negedge CLOCK) out_reg <= int_reg;
	assign OUT = out_reg;
endmodule

module DFFR(input IN1, input CLOCK, input ARESET, output OUT);
	reg int_reg;
	reg out_reg;
	always_ff @(posedge CLOCK or posedge ARESET)
		int_reg <= ARESET ? 1'b 0 : IN1;
	always_ff @(negedge CLOCK or posedge ARESET)
		out_reg <= ARESET ? 1'b 0 : int_reg;
	assign OUT = out_reg;
endmodule

`endcelldefine

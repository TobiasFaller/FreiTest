`default_nettype none

module MACISH #(
	parameter integer SIZE = 2,
	parameter [(SIZE*SIZE)/4-1:0] [31:0] CONFIG
) (
	input CLOCK, input CLOCK_EN, input ARESET, input ZERO,
	input [SIZE-1:0] A, input [SIZE-1:0] B, output [2*SIZE-1:0] MAC_OUT
);
	reg ZERO_REG;
	reg [SIZE-1:0] A_reg, B_reg;
	reg [2*SIZE-1:0] ACCUM_reg;
	assign MAC_OUT = ACCUM_reg;

	wire [2*SIZE-1:0] MUL_OUT;
	MUL #(.SIZE(SIZE), .CONFIG(CONFIG)) mul (.A(A_reg), .B(B_reg), .O(MUL_OUT));

	always @(posedge CLOCK or posedge ARESET) begin
		if (ARESET) begin
			ZERO_REG <= 1'b 0;
			A_reg <= {SIZE{1'b 0}};
			B_reg <= {SIZE{1'b 0}};

			ACCUM_reg <= {2*SIZE{1'b 0}};
		end else if (CLOCK_EN) begin
			ZERO_REG <= ZERO;
			A_reg <= A;
			B_reg <= B;

			if (ZERO) ACCUM_reg <= MUL_OUT;
			else      ACCUM_reg <= ACCUM_reg + MUL_OUT;
		end
	end

endmodule

module MUL #(
	parameter integer SIZE = 2,
	parameter [(SIZE*SIZE)/4-1:0] [31:0] CONFIG
) (
	input [SIZE-1:0] A, input [SIZE-1:0] B, output [2*SIZE-1:0] O
);
	generate
		if (SIZE == 2) begin
			if (CONFIG[0] == 0) M0 mul(.A(A), .B(B), .O(O));
			if (CONFIG[0] == 1) M1 mul(.A(A), .B(B), .O(O));
			if (CONFIG[0] == 2) M2 mul(.A(A), .B(B), .O(O));
			if (CONFIG[0] == 3) M3 mul(.A(A), .B(B), .O(O));
			if (CONFIG[0] == 4) M4 mul(.A(A), .B(B), .O(O));
		end else begin
			wire [SIZE/2-1:0] A_H, A_L;
			wire [SIZE/2-1:0] B_H, B_L;
			wire [SIZE-1:0] MUL0_OUT, MUL1_OUT, MUL2_OUT, MUL3_OUT;
			assign { A_H, A_L } = A;
			assign { B_H, B_L } = B;
			MUL #(.SIZE(SIZE/2), .CONFIG(CONFIG[$size(CONFIG)*1/4-1:$size(CONFIG)*0/4])) mul0 (.A(A_L), .B(B_L), .O(MUL0_OUT));
			MUL #(.SIZE(SIZE/2), .CONFIG(CONFIG[$size(CONFIG)*2/4-1:$size(CONFIG)*1/4])) mul1 (.A(A_H), .B(B_L), .O(MUL1_OUT));
			MUL #(.SIZE(SIZE/2), .CONFIG(CONFIG[$size(CONFIG)*3/4-1:$size(CONFIG)*2/4])) mul2 (.A(A_L), .B(B_H), .O(MUL2_OUT));
			MUL #(.SIZE(SIZE/2), .CONFIG(CONFIG[$size(CONFIG)*4/4-1:$size(CONFIG)*3/4])) mul3 (.A(A_H), .B(B_H), .O(MUL3_OUT));
			assign O = { MUL3_OUT, {SIZE{1'b 0}} }
				+ { {SIZE/2{1'b 0}}, MUL2_OUT, {SIZE/2{1'b 0}} }
				+ { {SIZE/2{1'b 0}}, MUL1_OUT, {SIZE/2{1'b 0}} }
				+ { {SIZE{1'b 0}}, MUL0_OUT };
		end
	endgenerate

endmodule

module M0(input [1:0] A, input [1:0] B, output [3:0] O);
    wire [3:0] level1 = { A[1] & B[1], A[1] & B[0], A[0] & B[1], A[0] & B[0] };
    wire [0:0] level2 = { level1[2] & level1[1] };
    wire [2:0] level3 = { level1[3] & level2[0], level2[0] ^ level1[3], level1[2] ^ level1[1] };
    assign O = { level3[2], level3[1], level3[0], level1[0] };
endmodule

module M1(input [1:0] A, input [1:0] B, output [3:0] O);
    wire [3:0] level1 = { A[1] & B[1], A[1] & B[0], A[0] & B[1], A[0] & B[0] };
    wire [0:0] level2 = { level1[1] | level1[2] };
    assign O = { 1'b 0, level1[3], level2[0], level1[0] };
endmodule

module M2(input [1:0] A, input [1:0] B, output [3:0] O);
    wire [2:0] level1 = { A[1] & B[1], A[1] & B[0], A[0] & B[1] };
    wire [1:0] level2 = { level1[0] ^ level1[1], level1[0] & level1[1] };
    wire [0:0] level3 = { level2[0] ^ level1[2] };
    assign O = { level2[0], level3[0], level2[1], level2[0] };
endmodule

module M3(input [1:0] A, input [1:0] B, output [3:0] O);
    wire [3:0] level1 = { A[1] & B[1], A[1] & B[0], A[0] & B[1], A[0] & B[0] };
    wire [0:0] level2 = { level1[1] | level1[2] };
    wire [1:0] level3 = { level1[3] & level1[0], (~level1[0]) & level1[3] };
    assign O = { level3[1], level3[0], level2[0], level1[0] };
endmodule

module M4(input [1:0] A, input [1:0] B, output [3:0] O);
    wire [3:0] level1 = { A[1] & B[1], A[1] & B[0], A[0] & B[1], A[0] & B[0] };
    wire [0:0] level2 = { level1[1] ^ level1[2] };
    assign O = { 1'b 0, level1[3], level2[0], level1[0] };
endmodule

// -----------------------------------------------------------------------------
// Preconfigured implementations below:

`define EXPAND(CONFIG) { \
	{(64'h F & (CONFIG >> 60))}[31:0], {(64'h F & (CONFIG >> 56))}[31:0], {(64'h F & (CONFIG >> 52))}[31:0], {(64'h F & (CONFIG >> 48))}[31:0], \
	{(64'h F & (CONFIG >> 44))}[31:0], {(64'h F & (CONFIG >> 40))}[31:0], {(64'h F & (CONFIG >> 36))}[31:0], {(64'h F & (CONFIG >> 32))}[31:0], \
	{(64'h F & (CONFIG >> 28))}[31:0], {(64'h F & (CONFIG >> 24))}[31:0], {(64'h F & (CONFIG >> 20))}[31:0], {(64'h F & (CONFIG >> 16))}[31:0], \
	{(64'h F & (CONFIG >> 12))}[31:0], {(64'h F & (CONFIG >>  8))}[31:0], {(64'h F & (CONFIG >>  4))}[31:0], {(64'h F & (CONFIG >>  0))}[31:0] \
}
`define MUL8_ACCURATE `EXPAND(64'h 0000000000000000)
`define MUL8_ONLY1    `EXPAND(64'h 1111111111111111)
`define MUL8_ONLY2    `EXPAND(64'h 2222222222222222)
`define MUL8_ONLY3    `EXPAND(64'h 3333333333333333)
`define MUL8_ONLY4    `EXPAND(64'h 4444444444444444)
`define MUL8_UNDERD   `EXPAND(64'h 1111111111111111)
`define MUL8_CONVEN1  `EXPAND(64'h 1000110010100111)
`define MUL8_CONVEN2  `EXPAND(64'h 1000000000100000)
`define MUL8_ISH1     `EXPAND(64'h 4143111114111114)
`define MUL8_ISH2     `EXPAND(64'h 1310140113440114)

`define MUL16_ACCURATE { `MUL8_ACCURATE,  `MUL8_ACCURATE,  `MUL8_ACCURATE,  `MUL8_ACCURATE  }
`define MUL16_UNDERD   { `MUL8_UNDERD,    `MUL8_UNDERD,    `MUL8_UNDERD,    `MUL8_UNDERD    }
`define MUL16_CONVEN1  { `MUL8_CONVEN1,   `MUL8_CONVEN1,   `MUL8_CONVEN1,   `MUL8_CONVEN1   }
`define MUL16_CONVEN2  { `MUL8_CONVEN2,   `MUL8_CONVEN2,   `MUL8_CONVEN2,   `MUL8_CONVEN2   }
`define MUL16_ISH1     { `MUL8_ISH1,      `MUL8_ISH1,      `MUL8_ISH1,      `MUL8_ISH1      }
`define MUL16_ISH2     { `MUL8_ISH2,      `MUL8_ISH2,      `MUL8_ISH2,      `MUL8_ISH2      }

`define MUL32_ACCURATE { `MUL16_ACCURATE, `MUL16_ACCURATE, `MUL16_ACCURATE, `MUL16_ACCURATE }
`define MUL32_UNDERD   { `MUL16_UNDERD,   `MUL16_UNDERD,   `MUL16_UNDERD,   `MUL16_UNDERD   }
`define MUL32_CONVEN1  { `MUL16_CONVEN1,  `MUL16_CONVEN1,  `MUL16_CONVEN1,  `MUL16_CONVEN1  }
`define MUL32_CONVEN2  { `MUL16_CONVEN2,  `MUL16_CONVEN2,  `MUL16_CONVEN2,  `MUL16_CONVEN2  }
`define MUL32_ISH1     { `MUL16_ISH1,     `MUL16_ISH1,     `MUL16_ISH1,     `MUL16_ISH1     }
`define MUL32_ISH2     { `MUL16_ISH2,     `MUL16_ISH2,     `MUL16_ISH2,     `MUL16_ISH2     }

`define MUL64_ACCURATE { `MUL32_ACCURATE, `MUL32_ACCURATE, `MUL32_ACCURATE, `MUL32_ACCURATE }
`define MUL64_UNDERD   { `MUL32_UNDERD,   `MUL32_UNDERD,   `MUL32_UNDERD,   `MUL32_UNDERD   }
`define MUL64_CONVEN1  { `MUL32_CONVEN1,  `MUL32_CONVEN1,  `MUL32_CONVEN1,  `MUL32_CONVEN1  }
`define MUL64_CONVEN2  { `MUL32_CONVEN2,  `MUL32_CONVEN2,  `MUL32_CONVEN2,  `MUL32_CONVEN2  }
`define MUL64_ISH1     { `MUL32_ISH1,     `MUL32_ISH1,     `MUL32_ISH1,     `MUL32_ISH1     }
`define MUL64_ISH2     { `MUL32_ISH2,     `MUL32_ISH2,     `MUL32_ISH2,     `MUL32_ISH2     }

`define ALIAS(NAME_MUL, NAME_MACISH, PSIZE, PCONF) \
	module NAME_MUL(input [PSIZE-1:0] A, input [PSIZE-1:0] B, output [2*PSIZE-1:0] O); \
		MUL #(.SIZE(PSIZE), .CONFIG(PCONF)) mul (.*); \
	endmodule \
	module NAME_MACISH( \
		input CLOCK, input CLOCK_EN, input ARESET, input ZERO, \
		input [PSIZE-1:0] A, input [PSIZE-1:0] B, output [2*PSIZE-1:0] MAC_OUT \
	); \
		MACISH #(.SIZE(PSIZE), .CONFIG(PCONF)) mac (.*); \
	endmodule

`ALIAS(MUL8_ACCURATE,  MACISH8_ACCURATE,  8,  `MUL8_ACCURATE )
`ALIAS(MUL8_UNDERD,    MACISH8_UNDERD,    8,  `MUL8_UNDERD   )
`ALIAS(MUL8_CONVEN1,   MACISH8_CONVEN1,   8,  `MUL8_CONVEN1  )
`ALIAS(MUL8_CONVEN2,   MACISH8_CONVEN2,   8,  `MUL8_CONVEN2  )
`ALIAS(MUL8_ISH1,      MACISH8_ISH1,      8,  `MUL8_ISH1     )
`ALIAS(MUL8_ISH2,      MACISH8_ISH2,      8,  `MUL8_ISH2     )

`ALIAS(MUL16_ACCURATE, MACISH16_ACCURATE, 16, `MUL16_ACCURATE)
`ALIAS(MUL16_UNDERD,   MACISH16_UNDERD,   16, `MUL16_UNDERD  )
`ALIAS(MUL16_CONVEN1,  MACISH16_CONVEN1,  16, `MUL16_CONVEN1 )
`ALIAS(MUL16_CONVEN2,  MACISH16_CONVEN2,  16, `MUL16_CONVEN2 )
`ALIAS(MUL16_ISH1,     MACISH16_ISH1,     16, `MUL16_ISH1    )
`ALIAS(MUL16_ISH2,     MACISH16_ISH2,     16, `MUL16_ISH2    )

`ALIAS(MUL32_ACCURATE, MACISH32_ACCURATE, 32, `MUL32_ACCURATE)
`ALIAS(MUL32_UNDERD,   MACISH32_UNDERD,   32, `MUL32_UNDERD  )
`ALIAS(MUL32_CONVEN1,  MACISH32_CONVEN1,  32, `MUL32_CONVEN1 )
`ALIAS(MUL32_CONVEN2,  MACISH32_CONVEN2,  32, `MUL32_CONVEN2 )
`ALIAS(MUL32_ISH1,     MACISH32_ISH1,     32, `MUL32_ISH1    )
`ALIAS(MUL32_ISH2,     MACISH32_ISH2,     32, `MUL32_ISH2    )

`ALIAS(MUL64_ACCURATE, MACISH64_ACCURATE, 64, `MUL64_ACCURATE)
`ALIAS(MUL64_UNDERD,   MACISH64_UNDERD,   64, `MUL64_UNDERD  )
`ALIAS(MUL64_CONVEN1,  MACISH64_CONVEN1,  64, `MUL64_CONVEN1 )
`ALIAS(MUL64_CONVEN2,  MACISH64_CONVEN2,  64, `MUL64_CONVEN2 )
`ALIAS(MUL64_ISH1,     MACISH64_ISH1,     64, `MUL64_ISH1    )
`ALIAS(MUL64_ISH2,     MACISH64_ISH2,     64, `MUL64_ISH2    )

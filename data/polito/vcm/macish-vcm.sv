`default_nettype none

module MACISH_VCM #(
	parameter integer SIZE
) (
	input               CLOCK_g,
	input               CLOCK_EN_g,
	input               ARESET_g,
	input               ZERO_g,
	input  [SIZE-1  :0] A_g,
	input  [SIZE-1  :0] B_g,
	input  [2*SIZE-1:0] MAC_OUT_g,
	input  [2*SIZE-1:0] MAC_OUT_b,
	output [2*SIZE-1:0] MAC_OUT_result,

	output [2*SIZE  :0] MAC_OUT_exact_result,
	output [2*SIZE  :0] MAC_OUT_faulty_result,
	output [2*SIZE  :0] MAC_OUT_faultfree_result,
	output [2*SIZE  :0] MAC_DIFFP_faulty_result,
	output [2*SIZE  :0] MAC_DIFFN_faulty_result,
	output [2*SIZE  :0] MAC_DIFFP_faultfree_result,
	output [2*SIZE  :0] MAC_DIFFN_faultfree_result
);

	reg ZERO_REG;
	reg [SIZE-1:0] A_reg, B_reg;
	reg [2*SIZE-1:0] ACCUM_reg;
	assign MAC_OUT_result = ACCUM_reg;

	wire [2*SIZE-1:0] MUL_OUT = A_reg * B_reg;

	always @(posedge CLOCK_g or posedge ARESET_g) begin
		if (ARESET_g) begin
			ZERO_REG <= 1'b 0;
			A_reg <= {SIZE{1'b 0}};
			B_reg <= {SIZE{1'b 0}};

			ACCUM_reg <= {2*SIZE{1'b 0}};
		end else if (CLOCK_EN_g) begin
			ZERO_REG <= ZERO_g;
			A_reg <= A_g;
			B_reg <= B_g;

			if (ZERO_g) ACCUM_reg <= MUL_OUT;
			else        ACCUM_reg <= ACCUM_reg + MUL_OUT;
		end
	end

	wire [2*SIZE:0] MAC_OUT_EXT_g      = {1'b 0, MAC_OUT_g};
	wire [2*SIZE:0] MAC_OUT_EXT_b      = {1'b 0, MAC_OUT_b};
	wire [2*SIZE:0] MAC_OUT_EXT_result = {1'b 0, MAC_OUT_result};

	assign MAC_OUT_exact_result = MAC_OUT_EXT_result;
	assign MAC_OUT_faulty_result = MAC_OUT_EXT_b;
	assign MAC_OUT_faultfree_result = MAC_OUT_EXT_g;
	assign MAC_DIFFP_faulty_result = MAC_OUT_EXT_result - MAC_OUT_EXT_b;
	assign MAC_DIFFP_faultfree_result = MAC_OUT_EXT_result - MAC_OUT_EXT_g;
	assign MAC_DIFFN_faulty_result = MAC_OUT_EXT_b - MAC_OUT_EXT_result;
	assign MAC_DIFFN_faultfree_result = MAC_OUT_EXT_g - MAC_OUT_EXT_result;

endmodule

module MUL_VCM #(
	parameter integer SIZE
) (
	input  [SIZE-1  :0] A_g,
	input  [SIZE-1  :0] B_g,
	input  [2*SIZE-1:0] O_g,
	input  [2*SIZE-1:0] O_b,
	output [2*SIZE-1:0] O_result,

	output [2*SIZE  :0] MUL_OUT_exact_result,
	output [2*SIZE  :0] MUL_OUT_faulty_result,
	output [2*SIZE  :0] MUL_OUT_faultfree_result,
	output [2*SIZE  :0] MUL_DIFFP_faulty_result,
	output [2*SIZE  :0] MUL_DIFFN_faulty_result,
	output [2*SIZE  :0] MUL_DIFFP_faultfree_result,
	output [2*SIZE  :0] MUL_DIFFN_faultfree_result
);

	assign O_result = A_g * B_g;

	wire [2*SIZE:0] O_EXT_g      = {1'b 0, O_g};
	wire [2*SIZE:0] O_EXT_b      = {1'b 0, O_b};
	wire [2*SIZE:0] O_EXT_result = {1'b 0, O_result};

	assign MUL_OUT_exact_result = O_EXT_result;
	assign MUL_OUT_faulty_result = O_EXT_b;
	assign MUL_OUT_faultfree_result = O_EXT_g;
	assign MUL_DIFFP_faulty_result = O_EXT_result - O_EXT_b;
	assign MUL_DIFFP_faultfree_result = O_EXT_result - O_EXT_g;
	assign MUL_DIFFN_faulty_result = O_EXT_b - O_EXT_result;
	assign MUL_DIFFN_faultfree_result = O_EXT_g - O_EXT_result;

endmodule

`define ALIAS(MACISH_NAME, MUL_NAME, PSIZE) \
	module MUL_NAME ( \
		input  [PSIZE-1  :0] A_g, \
		input  [PSIZE-1  :0] B_g, \
		input  [2*PSIZE-1:0] O_g, \
		input  [2*PSIZE-1:0] O_b, \
		output [2*PSIZE-1:0] O_result, \
		output [2*PSIZE  :0] MUL_OUT_exact_result, \
		output [2*PSIZE  :0] MUL_OUT_faulty_result, \
		output [2*PSIZE  :0] MUL_OUT_faultfree_result, \
		output [2*PSIZE  :0] MUL_DIFFP_faulty_result, \
		output [2*PSIZE  :0] MUL_DIFFN_faulty_result, \
		output [2*PSIZE  :0] MUL_DIFFP_faultfree_result, \
		output [2*PSIZE  :0] MUL_DIFFN_faultfree_result \
	); \
		MUL_VCM #(.SIZE(PSIZE)) vcm (.*); \
	endmodule \
	module MACISH_NAME ( \
		input                CLOCK_g, \
		input                CLOCK_EN_g, \
		input                ARESET_g, \
		input                ZERO_g, \
		input  [PSIZE-1  :0] A_g, \
		input  [PSIZE-1  :0] B_g, \
		input  [2*PSIZE-1:0] MAC_OUT_g, \
		input  [2*PSIZE-1:0] MAC_OUT_b, \
		output [2*PSIZE-1:0] MAC_OUT_result, \
		output [2*PSIZE  :0] MAC_OUT_exact_result, \
		output [2*PSIZE  :0] MAC_OUT_faulty_result, \
		output [2*PSIZE  :0] MAC_OUT_faultfree_result, \
		output [2*PSIZE  :0] MAC_DIFFP_faulty_result, \
		output [2*PSIZE  :0] MAC_DIFFN_faulty_result, \
		output [2*PSIZE  :0] MAC_DIFFP_faultfree_result, \
		output [2*PSIZE  :0] MAC_DIFFN_faultfree_result \
	); \
		MACISH_VCM #(.SIZE(PSIZE)) vcm (.*); \
	endmodule

`ALIAS(MACISH8_VCM,  MUL8_VCM,  8 )
`ALIAS(MACISH16_VCM, MUL16_VCM, 16)
`ALIAS(MACISH32_VCM, MUL32_VCM, 32)
`ALIAS(MACISH64_VCM, MUL64_VCM, 64)

`default_nettype none

`include "./macish.v"

`define max(A, B) (((A) > (B)) ? (A) : (B))
`define min(A, B) (((A) < (B)) ? (A) : (B))
`define diff(A, B) (`max(A, B) - `min(A, B))
`define sqr(A) ((A) * (A))
`define abs(A) (((A) < 0.0) ? (-A) : (A))
`define assess_delta(A, B, O, EXPECTED, ERR_RATE, ERR_MEAN, ERR_MEANN, ERR_MAX, ERR_MAXN, ERR_OVERFLOW, ERR_UNDERFLOW) \
	if (integer'(O) !== integer'(EXPECTED)) begin \
		ERR_RATE = ERR_RATE + 1.0; \
		ERR_MEAN = ERR_MEAN + ((real'(integer'(O)) - real'(integer'(EXPECTED))) / real'(integer'(EXPECTED))); \
		ERR_MEANN = ERR_MEANN + ((real'(integer'(O)) - real'(integer'(EXPECTED))) / real'(2**(2*SIZE)-1)); \
		ERR_MAX = `max(ERR_MAX, `diff(real'(integer'(O)), real'(integer'(EXPECTED))) / real'(integer'(EXPECTED))); \
		ERR_MAXN = `max(ERR_MAXN, `diff(real'(integer'(O)), real'(integer'(EXPECTED))) / real'(2**(2*SIZE)-1)); \
		if ((integer'(EXPECTED) >= ((2**(2*SIZE-2))*3-1)) && (integer'(O) <= ((2**(2*SIZE-2))*1-1))) begin \
			ERR_OVERFLOW = ERR_OVERFLOW + 1.0; \
		end \
		if ((integer'(EXPECTED) <= ((2**(2*SIZE-2))*1-1)) && (integer'(O) >= ((2**(2*SIZE-2))*3-1))) begin \
			ERR_UNDERFLOW = ERR_UNDERFLOW + 1.0; \
		end \
	end

module MUL_tb();

`ifdef MUL8_TB
	localparam integer SIZE = 8;
`endif
`ifdef MUL16_TB
	localparam integer SIZE = 16;
`endif
`ifdef MUL32_TB
	localparam integer SIZE = 32;
`endif

	localparam integer SEED = 30974234;
	localparam integer UNIFORM_SAMPLES = 1000000;
	localparam integer NORMAL_SAMPLES = 10000000;
	localparam integer NORMAL_MU = 2**(SIZE-1);
	localparam integer NORMAL_SIGMA = 2**(SIZE-1) / 5;

	/* verilator lint_off ASCRANGE */
`ifdef MUL8_TB
	localparam [0:5] [15:0] [31:0] CONFIGS = {
		`MUL8_ACCURATE,
		//`MUL8_ONLY1,
		//`MUL8_ONLY2,
		//`MUL8_ONLY3,
		//`MUL8_ONLY4,
		`MUL8_UNDERD,
		`MUL8_CONVEN1,
		`MUL8_CONVEN2,
		`MUL8_ISH1,
		`MUL8_ISH2
	};
	string NAMES [0:$size(CONFIGS)-1] = {
		"ACCURATE",
		//"ONLY1   ",
		//"ONLY2   ",
		//"ONLY3   ",
		//"ONLY4   ",
		"UNDERD  ",
		"CONVEN1 ",
		"CONVEN2 ",
		"ISH1    ",
		"ISH2    "
	};
`endif
`ifdef MUL16_TB
	localparam [0:5] [63:0] [31:0] CONFIGS = {
		`MUL16_ACCURATE,
		`MUL16_UNDERD,
		`MUL16_CONVEN1,
		`MUL16_CONVEN2,
		`MUL16_ISH1,
		`MUL16_ISH2
	};
	string NAMES [0:$size(CONFIGS)-1] = {
		"ACCURATE",
		"UNDERD  ",
		"CONVEN1 ",
		"CONVEN2 ",
		"ISH1    ",
		"ISH2    "
	};
`endif
`ifdef MUL32_TB
	localparam [0:5] [255:0] [31:0] CONFIGS = {
		`MUL32_ACCURATE,
		`MUL32_UNDERD,
		`MUL32_CONVEN1,
		`MUL32_CONVEN2,
		`MUL32_ISH1,
		`MUL32_ISH2
	};
	string NAMES [0:$size(CONFIGS)-1] = {
		"ACCURATE",
		"UNDERD  ",
		"CONVEN1 ",
		"CONVEN2 ",
		"ISH1    ",
		"ISH2    "
	};
`endif
	/* verilator lint_on ASCRANGE */

	integer count_started [$size(CONFIGS)] = '{default:0};
	integer count_finished [$size(CONFIGS)] = '{default:0};
	real count_uniform [$size(CONFIGS)] = '{default:0.0},
		error_rate_uniform [$size(CONFIGS)] = '{default:0.0},
		error_mean_uniform [$size(CONFIGS)] = '{default:0.0},
		error_meann_uniform [$size(CONFIGS)] = '{default:0.0},
		error_max_uniform [$size(CONFIGS)] = '{default:0.0},
		error_maxn_uniform [$size(CONFIGS)] = '{default:0.0},
		error_overflow_uniform [$size(CONFIGS)] = '{default:0.0},
		error_underflow_uniform [$size(CONFIGS)] = '{default:0.0};
	real count_normal [$size(CONFIGS)] = '{default:0.0},
		error_rate_normal [$size(CONFIGS)] = '{default:0.0},
		error_mean_normal [$size(CONFIGS)] = '{default:0.0},
		error_meann_normal [$size(CONFIGS)] = '{default:0.0},
		error_max_normal [$size(CONFIGS)] = '{default:0.0},
		error_maxn_normal [$size(CONFIGS)] = '{default:0.0},
		error_overflow_normal [$size(CONFIGS)] = '{default:0.0},
		error_underflow_normal [$size(CONFIGS)] = '{default:0.0};

	function integer clip (integer value, integer min, integer max);
		if (value < min) begin
			clip = min;
		end else if (value > max) begin
			clip = max;
		end else begin
			clip = value;
		end
	endfunction

	generate
		for (genvar i = 0; i < $size(CONFIGS); i++) begin
			reg  [SIZE-1:0] A, B;
			wire [2*SIZE-1:0] O;
			MUL #(.SIZE(SIZE), .CONFIG(CONFIGS[i])) mul (A, B, O);

			integer seed = SEED;
			initial begin
				count_started[i] = 1;

				for (integer j = 0; j < UNIFORM_SAMPLES; j++) begin
					A = {$dist_uniform(seed, 0, 2**SIZE-1)}[SIZE-1:0];
					B = {$dist_uniform(seed, 0, 2**SIZE-1)}[SIZE-1:0];
					count_uniform[i] = count_uniform[i] + 1.0; #1;
					`assess_delta(A, B, O, A * B,
						error_rate_uniform[i],
						error_mean_uniform[i],
						error_meann_uniform[i],
						error_max_uniform[i],
						error_maxn_uniform[i],
						error_overflow_uniform[i],
						error_underflow_uniform[i]);
				end

				for (integer j = 0; j < NORMAL_SAMPLES; j++) begin
					A = {clip($dist_normal(seed, 0, NORMAL_SIGMA) + NORMAL_MU, 0, 2**SIZE-1)}[SIZE-1:0];
					B = {clip($dist_normal(seed, 0, NORMAL_SIGMA) + NORMAL_MU, 0, 2**SIZE-1)}[SIZE-1:0];
					count_normal[i] = count_normal[i] + 1.0; #1;
					`assess_delta(A, B, O, A * B,
						error_rate_normal[i],
						error_mean_normal[i],
						error_meann_normal[i],
						error_max_normal[i],
						error_maxn_normal[i],
						error_overflow_normal[i],
						error_underflow_normal[i]);
				end

				count_finished[i] = 1;
			end
		end
	endgenerate

	initial begin
		$display("%h", ((2**(2*SIZE-2))*1-1));
		$display("%h", ((2**(2*SIZE-2))*3-1));
		for (integer i = 0; i < $size(CONFIGS); i++) begin
			wait (count_finished[i] == 1);
		end
		for (integer i = 0; i < $size(CONFIGS); i++) begin
			$display("UNIFORM %s e-rate: %.3f %%, e-mean: %.10f %%, e-max: %.3f %%, e-mean-norm: %.3e, e-max-norm: %.3f %%, overflow: %.0f, underflow: %.0f", NAMES[i],
				error_rate_uniform[i] / count_uniform[i] * 100.0,
				error_mean_uniform[i] / count_uniform[i] * 100.0,
				error_max_uniform[i] * 100.0,
				error_meann_uniform[i] / count_uniform[i] ,
				error_maxn_uniform[i] * 100.0,
				error_overflow_uniform[i],
				error_underflow_uniform[i]);
		end
		for (integer i = 0; i < $size(CONFIGS); i++) begin
			$display("NORMAL  %s e-rate: %.3f %%, e-mean: %.10f %%, e-max: %.3f %%, e-mean-norm: %.3e, e-max-norm: %.3f %%, overflow: %.0f, underflow: %.0f", NAMES[i],
				error_rate_normal[i] / count_normal[i] * 100.0,
				error_mean_normal[i] / count_normal[i] * 100.0,
				error_max_normal[i] * 100.0,
				error_meann_normal[i] / count_normal[i],
				error_maxn_normal[i] * 100.0,
				error_overflow_normal[i],
				error_underflow_normal[i]);
		end
		$finish;
	end
endmodule

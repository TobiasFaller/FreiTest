`default_nettype none

`include "./macish.v"

`define assert_exact(A, B, O, EXPECTED) \
	if (O !== EXPECTED) begin \
		$display("FAILED in %m: O != EXPECTED for A=0x%0h, B=0x%0h O=0x%0h", A, B, O); \
		$finish; \
	end

module M0_tb();
	reg  [1:0] A;
	reg  [1:0] B;
	wire [3:0] O;
	M0 mul (A, B, O);
	initial begin
		A = 2'b 00; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 11; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 00; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0001);
		A = 2'b 10; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 11; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 00; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0100);
		A = 2'b 11; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 00; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 10; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 11; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 1001);
		$display("PASSED");
		$finish;
	end
endmodule

module M1_tb();
	reg  [1:0] A;
	reg  [1:0] B;
	wire [3:0] O;
	M1 mul (A, B, O);
	initial begin
		A = 2'b 00; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 11; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 00; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0001);
		A = 2'b 10; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 11; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 00; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0100);
		A = 2'b 11; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 00; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 10; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 11; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0111);
		$display("PASSED");
		$finish;
	end
endmodule

module M2_tb();
	reg  [1:0] A;
	reg  [1:0] B;
	wire [3:0] O;
	M2 mul (A, B, O);
	initial begin
		A = 2'b 00; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 11; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 00; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 11; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 00; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0100);
		A = 2'b 11; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 00; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 11; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 1001);
		$display("PASSED");
		$finish;
	end
endmodule

module M3_tb();
	reg  [1:0] A;
	reg  [1:0] B;
	wire [3:0] O;
	M3 mul (A, B, O);
	initial begin
		A = 2'b 00; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 11; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 00; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0001);
		A = 2'b 10; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 11; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 00; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0100);
		A = 2'b 11; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 00; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 10; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 11; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 1011);
		$display("PASSED");
		$finish;
	end
endmodule

module M4_tb();
	reg  [1:0] A;
	reg  [1:0] B;
	wire [3:0] O;
	M4 mul (A, B, O);
	initial begin
		A = 2'b 00; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 10; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 11; B = 2'b 00; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 00; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0001);
		A = 2'b 10; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 11; B = 2'b 01; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 00; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0010);
		A = 2'b 10; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0100);
		A = 2'b 11; B = 2'b 10; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 00; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0000);
		A = 2'b 01; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0011);
		A = 2'b 10; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0110);
		A = 2'b 11; B = 2'b 11; #1; `assert_exact(A, B, O, 4'b 0101);
		$display("PASSED");
		$finish;
	end
endmodule

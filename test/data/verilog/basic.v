module basic_test
(
    input [7:0] A,
    input [7:0] B,
    output [7:0] OUT
);

    and g1(OUT[0], A[0], B[0]);
    or g2(OUT[1], A[1], B[1]);
    xor g3(OUT[2], A[2], B[2]);
    nand g4(OUT[3], A[3], B[3]);
    nor g5(OUT[4], A[4], B[4]);
    buf g6(OUT[5], A[5]);
    not g7(OUT[6], A[6]);
    not g8(OUT[7], B[7]);

endmodule

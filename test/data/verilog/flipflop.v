module flip_flop(clock, data_in, data_out);
    input clock;
    input data_in;
    output data_out;

    dflipflop(.CK(clock), .D(data_in), .Q(data_out));

endmodule

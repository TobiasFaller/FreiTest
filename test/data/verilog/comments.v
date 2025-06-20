/*
 * This is a small verilog module to test if the processor is working correctly.
 * This comment should not appear in the result.
 */
module test(
    input clk,
    // This is another comment which should be removed
    output [15:0] counter
);

    reg [15:0] /* Here could be your advertisement! */ reg_counter;

    always @(posedge clk)
    begin
        	reg_counter <= reg_counter + 16'h 0001;
    end

    assign counter = reg_counter;

endmodule

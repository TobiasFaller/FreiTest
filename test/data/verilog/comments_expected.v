module test(
input clk,
output [15:0] counter
);
reg [15:0] reg_counter;
always @(posedge clk)
begin
reg_counter <= reg_counter + 16'h 0001;
end
assign counter = reg_counter;
endmodule

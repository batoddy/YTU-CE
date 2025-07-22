`timescale 1ns / 1ns
`include "adder.v"
module add4_tb;

    // Inputs
    reg [3:0] a, b;
    reg ci;
    
    // Outputs
    wire [3:0] s;
    wire co;
    
    add4 uut (
        .a(a),
        .b(b),
        .ci(ci),
        .s(s),
        .co(co)
    );
    
    initial begin
        // Test case 1
        $display("Test Case 1: a = 4'b0000, b = 4'b0000, ci = 0");
        a = 4'b0000;
        b = 4'b0000;
        ci = 1'b0;
        #10;
        // Test case 2
        $display("Test Case 2: a = 4'b1010, b = 4'b0101, ci = 0");
        a = 4'b1010;
        b = 4'b0101;
        ci = 1'b0;
        #10;
        // Test case 3
        $display("Test Case 3: a = 4'b1111, b = 4'b1111, ci = 1");
        a = 4'b1111;
        b = 4'b1111;
        ci = 1'b1;
        #10;
        $finish;
    end

initial begin
        $dumpfile("add4_tb.vcd");
        $dumpvars(0, add4_tb);
end
endmodule

//iverilog -o adder_tb.vvp adder_tb.v
//vvp adder_tb.vvp
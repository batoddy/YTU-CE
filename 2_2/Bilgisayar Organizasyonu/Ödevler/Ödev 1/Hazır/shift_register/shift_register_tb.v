`timescale 1ns/1ns
`include "shift_register_bh.v"
module shift_register_tb;

    // Input
    reg clk;
    reg reset;
    reg data_in;

    // Output
    wire [3:0] data_out;

    shift_register_bh uut (
        .clk(clk), 
        .reset(reset), 
        .data_in(data_in), 
        .data_out(data_out)
    );

    initial begin
        clk = 0;
        reset = 1;
        data_in = 0;

        #50;
        
        reset = 1;
        #10;
        reset = 0;
        #10;
        // Add 1s as input
        data_in = 1;
        #20;
        // Add 0s as input
        data_in = 0;
        #40;
        
        $finish;
    end
    
    always #5 clk = ~clk; // 10ns clk cycle



    initial begin
        $dumpfile("shift_register_tb.vcd");
        $dumpvars(0, shift_register_tb);
        #0;
    end
endmodule

//iverilog -o shift_register_tb.vvp shift_register_tb.v
//vvp shift_register_tb.vvp
`timescale 1ns / 1ps
`include "counter_fb.v"

module counter_fb_tb;

    // Inputs
    reg CLK, RESET, EN, DIR;
    
    // Outputs
    wire [3:0] Q;
    wire CO;
    
    counter_fb uut (
        .CLK(CLK),
        .RESET(RESET),
        .EN(EN),
        .DIR(DIR),
        .Q(Q),
        .CO(CO)
    );
    
    // Clock generation
    always #5 CLK = ~CLK;

    initial begin
        CLK = 0;
        RESET = 1;
        EN = 1;
        DIR = 1; //Count up

        $dumpfile("counter_fb_tb.vcd");
        $dumpvars(0, counter_fb_tb);
        
        #10; RESET = 0;
        
        #50;
        
        DIR = 0; //Count down
        
        #50;
        
        $finish;
    end
endmodule

//iverilog -o counter_fb_tb.vvp counter_fb_tb.v
//vvp counter_fb_tb.vvp
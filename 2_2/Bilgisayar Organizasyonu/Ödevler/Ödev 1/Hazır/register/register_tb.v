

`timescale 1ns / 1ps
`include "register.v"
module register_tb;

    reg [3:0] A;
    reg clk;
    wire q0, q1, q2, q3;

    register uut (
        .A(A),
        .clk(clk),
        .q0(q0),
        .q1(q1),
        .q2(q2),
        .q3(q3)
    );

    // Clock
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        A = 4'b0000;
        #10;
        
        A = 4'b1010; #10;
        A = 4'b1100; #10;
        A = 4'b0011; #10;

        $finish;
    end

    initial begin
        $dumpfile("register_tb.vcd");
        $dumpvars(0, register_tb);
        #0;
    end
    
endmodule

//iverilog -o register_tb.vvp register_tb.v
//vvp register_tb.vvp
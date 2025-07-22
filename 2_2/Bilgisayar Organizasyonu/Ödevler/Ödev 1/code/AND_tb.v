`timescale 1ns / 1ps

`include "AND_gate.v"  

module test_AND_tb;

    // Inputs
    reg a;
    reg b;

    // Output
    wire y;

    // Instantiate the Unit Under Test (UUT)
    AND_tb uut (
        .a(a),
        .b(b),
        .y(y)
    );

    initial begin
        // Test case 1: a = 0, b = 0
        a = 0; b = 0;
        #10;
        $display("Test case 1: a = %b, b = %b, y = %b", a, b, y);

        // Test case 2: a = 0, b = 1
        a = 0; b = 1;
        #10;
        $display("Test case 2: a = %b, b = %b, y = %b", a, b, y);

        // Test case 3: a = 1, b = 0
        a = 1; b = 0;
        #10;
        $display("Test case 3: a = %b, b = %b, y = %b", a, b, y);

        // Test case 4: a = 1, b = 1
        a = 1; b = 1;
        #10;
        $display("Test case 4: a = %b, b = %b, y = %b", a, b, y);

        $stop;
    end

    initial begin
        $dumpfile("AND_tb.vcd");
        $dumpvars(0, test_AND_tb);
    end

endmodule
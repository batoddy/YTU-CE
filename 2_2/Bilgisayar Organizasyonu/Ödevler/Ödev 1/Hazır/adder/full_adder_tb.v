/* Test bench for full_adder.v */

`include "full_adder.v"

module full_adder_tb;
    reg a, b, cin;
    wire sum, cout;

    full_adder add1(a, b, cin, sum, cout);

    initial
        begin
            $dumpfile("full_adder.vcd");
            $dumpvars(0, full_adder_tb);
            a = 0; b = 0; cin = 0; #1
            a = 0; b = 0; cin = 1; #1
            a = 0; b = 1; cin = 0; #1
            a = 0; b = 1; cin = 1; #1
            a = 1; b = 0; cin = 0; #1
            a = 1; b = 0; cin = 1; #1
            a = 1; b = 1; cin = 0; #1
            a = 1; b = 1; cin = 1;
        end
endmodule

//iverilog -o full_adder_tb.vvp full_adder_tb.v
//vvp full_adder_tb.vvp
/* test bench for half_adder.v */

`include "half_adder.v"

module half_adder_tb;
    reg a, b;
    wire sum, carry;

    half_adder add1(a, b, sum, carry);

    initial
        begin
            // we need to dump the changes of sum & carry

            $dumpfile("half_adder.vcd"); // value changed dump file
            $dumpvars(0, half_adder_tb); // 0 = we are dumping all the variables instantiated by half_adder_tb
            a = 0; b = 0;
            #1 // after 1 timescale
            a = 0; b = 1;
            #1
            a = 1; b = 0;
            #1
            a = 1; b = 1;
        end
endmodule

//iverilog -o half_adder_tb.vvp half_adder_tb.v
//vvp half_adder_tb.vvp
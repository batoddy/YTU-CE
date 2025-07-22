`timescale 1ns/1ns

`include "mux_2x1_32_bh.v"

module mux_2x1_32_bh_tb;

    reg [31:0] a, b;
    reg select;

    wire [31:0] out;

    mux_2x1_32_bh UUT (
        .out(out),
        .a(a),
        .b(b),
        .select(select)
    );

    initial begin

        a = 32'hABCDEFF0;
        b = 32'h12345678;
        select = 1'b0;
        #10;
        select = 1'b1;
        #10;
        $finish;
    end

    // Display outputs
    always @* begin
        $display("Input a: %h, Input b: %h, Select: %b, Output: %h", a, b, select, out);
    end

initial begin
        $dumpfile("mux_2x1_32_bh_tb.vcd");
        $dumpvars(0, mux_2x1_32_bh_tb);
    end


endmodule

//iverilog -o mux_2x1_32_bh_tb.vvp mux_2x1_32_bh_tb.v
//vvp mux_2x1_32_bh_tb.vvp
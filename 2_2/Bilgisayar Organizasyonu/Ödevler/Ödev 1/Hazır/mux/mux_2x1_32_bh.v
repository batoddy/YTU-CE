module mux_2x1_32_bh (out, a, b, select); // multiplexer, 32 bits
    input [31:0] a, b; // inputs, 32 bits
    input select; // input, 1 bit
    output [31:0] out; // output, 32 bits
    assign out = select ? a : b; // if (select==1) out=a; else out=b;
endmodule

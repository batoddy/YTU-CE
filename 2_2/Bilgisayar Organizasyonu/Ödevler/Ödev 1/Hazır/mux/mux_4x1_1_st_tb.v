
`include "mux_4x1_1_st.v"
module mux_4x1_1_st_tb;

reg a, b, c, d;
reg [1:0] select;
wire out;

// 4x1
mux_4x1_1_st uut (
    .out(out), 
    .a(a), 
    .b(b), 
    .c(c), 
    .d(d), 
    .select(select)
);

initial begin
    
    a = 1'b0; b = 1'b0; c = 1'b0; d = 1'b0; select = 00;
    
    #10; a = 1'b0; b = 1'b0; c = 1'b0; d = 1'b1; select = 2'b00; // d
    #10; a = 1'b0; b = 1'b1; c = 1'b0; d = 1'b0; select = 2'b01; // b
    #10; a = 1'b0; b = 1'b0; c = 1'b1; d = 1'b0; select = 2'b10; // c
    #10; a = 1'b1; b = 1'b0; c = 1'b0; d = 1'b0; select = 2'b11; // a

    
    #10; $finish;
end

initial begin
    $monitor("Time = %t, select = %b, a = %b, b = %b, c = %b, d = %b, out = %b", $time, select, a, b, c, d, out);
end

initial begin
        $dumpfile("mux_4x1_1_st_tb.vcd");
        $dumpvars(0, mux_4x1_1_st_tb);
    end

endmodule

//iverilog -o mux_4x1_1_st_tb.vvp mux_4x1_1_st_tb.v
//vvp mux_4x1_1_st_tb.vvp


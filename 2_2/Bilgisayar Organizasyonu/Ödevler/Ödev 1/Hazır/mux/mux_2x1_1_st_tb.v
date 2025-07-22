
`include "mux_2x1_1_st.v"

module mux_2x1_1_st_tb();

    reg a, b, select;

    wire out;
    
    mux_2x1_1_st UUT(
        .out(out),
        .a(a),
        .b(b),
        .select(select)
    );
    
    initial begin
        select = 0; a = 0; b = 0;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 1; a = 0; b = 0;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 0; a = 0; b = 1;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 1; a = 0; b = 1;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 0; a = 1; b = 0;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 1; a = 1; b = 0;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 0; a = 1; b = 1;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        select = 1; a = 1; b = 1;
        #10 $display("select=%b a=%b b=%b out=%b", select, a, b, out);
        
        #10 $finish;
    end

initial begin
        $dumpfile("mux_2x1_1_st_tb.vcd");
        $dumpvars(0, mux_2x1_1_st_tb);
    end

endmodule

//iverilog -o mux_2x1_1_st_tb.vvp mux_2x1_1_st_tb.v
//vvp mux_2x1_1_st_tb.vvp
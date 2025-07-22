// DFlipFlop_st.v

module nand_gate(c,a,b);
    input a,b;
    output c;
    assign c= ~(a & b);
endmodule

module not_gate(f,e);
    input e;
    output f;
    assign f = ~e;
endmodule

module DFlipFlop_st(q,qbar,d,clk);
input d,clk; 
output q, qbar; 
not_gate not1(dbar,d); //not(dbar, d)
nand_gate nand1(x,clk,d); //nand(x,clk,d)
nand_gate nand2(y,clk,dbar); //nand(y,clk,dbar)
nand_gate nand3(q,qbar,x); //nand(q,qbar,x)
nand_gate nand4(qbar,q,y); //nand(qbar,q,y)
endmodule
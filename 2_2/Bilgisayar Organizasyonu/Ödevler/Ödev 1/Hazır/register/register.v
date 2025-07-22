`include "DFlipFlop_st.v"

module register(A,clk,q0,q1,q2,q3);
    input [0:3] A;
    input clk;
    output wire q0, q1, q2, q3; //explicit wire declaration to indicate they are driven by DFlipFlop_st instances
    wire qb0, qb1, qb2, qb3;
    reg d0, d1, d2, d3;

    DFlipFlop_st df0(q0, qb0, d0, clk);
    DFlipFlop_st df1(q1, qb1, d1, clk);
    DFlipFlop_st df2(q2, qb2, d2, clk);
    DFlipFlop_st df3(q3, qb3, d3, clk);

    always @(posedge clk)
        if (clk)
            begin
               d0 = A[0];
               d1 = A[1];
               d2 = A[2];
               d3 = A[3];
            end
endmodule
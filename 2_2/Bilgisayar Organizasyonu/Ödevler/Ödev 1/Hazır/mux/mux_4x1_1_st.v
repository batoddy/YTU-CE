`include "mux_2x1_1_st.v"

module mux_4x1_1_st(out, a, b, c, d, select);
    input a, b, c, d;
    input [1:0] select; //2-bitlik seçim bitleri
    output out;

    wire out1, out2;

    // first level 2x1 MUXes
    mux_2x1_1_st mux1( // mux1, a ve b girişlerini seçer
        .out(out1),
        .a(a),
        .b(b),
        .select(select[1])
    );

    mux_2x1_1_st mux2( // mux2, c ve d girişlerini seçer
        .out(out2),
        .a(c),
        .b(d),
        .select(select[1])
    );

    // second level 2x1 MUX
    mux_2x1_1_st mux3( // mux3, mux1 ve mux2 çıkışlarını seçer
        .out(out),
        .a(out1),
        .b(out2),
        .select(select[0])
    );
endmodule
module decoder_st(EN, A0, A1, D0, D1, D2, D3);
    input EN, A0, A1;
    output D0, D1, D2, D3;
    wire A0_n, A1_n, N0, N1, N2, N3;
    not
        NOT_0 (A0_n, A0),
        NOT_1 (A1_n, A1);
    and
        AND_1 (N0, A0_n, A1_n),
        AND_2 (N1, A0, A1_n) ,
        AND_3 (N2, A0_n, A1),
        AND_4 (N3 , A0 , A1) ,
        AND_5 (D0, N0, EN),
        AND_6 (D1, N1, EN),
        AND_7 (D2 , N2 , EN) ,
        AND_8 (D3, N3, EN);
endmodule
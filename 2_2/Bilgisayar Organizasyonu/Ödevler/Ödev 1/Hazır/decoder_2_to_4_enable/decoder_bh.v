module decoder_bh(
    input EN, A0, A1,
    output reg D0, D1, D2, D3
);

always @(*) begin
    if (EN == 1'b0) begin
        D0 = 1'b0;
        D1 = 1'b0;
        D2 = 1'b0;
        D3 = 1'b0;
    end else begin
        
        case({A1, A0})
            2'b00: begin
                D0 = 1'b1;
                D1 = 1'b0;
                D2 = 1'b0;
                D3 = 1'b0;
            end
            2'b01: begin
                D0 = 1'b0;
                D1 = 1'b1;
                D2 = 1'b0;
                D3 = 1'b0;
            end
            2'b10: begin
                D0 = 1'b0;
                D1 = 1'b0;
                D2 = 1'b1;
                D3 = 1'b0;
            end
            2'b11: begin
                D0 = 1'b0;
                D1 = 1'b0;
                D2 = 1'b0;
                D3 = 1'b1;
            end
            default: begin
                D0 = 1'b0;
                D1 = 1'b0;
                D2 = 1'b0;
                D3 = 1'b0;
            end
        endcase
    end
end

endmodule
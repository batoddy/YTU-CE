//4-bit Binary Counter with Reset

module counter (
    input CLK, RESET, EN,
    output reg [3:0] Q,
    output reg CO
);

always @* begin
    CO = (Q == 4'b1111 && EN == 1'b1) ? 1'b1 : 1'b0;
end

always @(posedge CLK or posedge RESET) begin
    if (RESET) begin
        Q <= 4'b0000;
    end
    else if (EN) begin
        Q <= Q + 4'b0001;
    end
end

endmodule
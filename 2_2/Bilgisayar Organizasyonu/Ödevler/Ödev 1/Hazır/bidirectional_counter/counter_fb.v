// 4-bit Binary Counter with Reset, Enable, and Direction control

module counter_fb (
    input CLK,     // Clock input
    input RESET,   // reset input
    input EN,      // Enable input
    input DIR,     // Direction (1 = up, 0 = down)
    output reg [3:0] Q,   // 4-bit output
    output reg CO         // Carry-out/overflow
);

//DIRECTION Carry
always @* begin
    if (DIR == 1'b1) // Count up
        CO = (Q == 4'b1111 && EN == 1'b1) ? 1'b1 : 1'b0;
    else // Count down
        CO = (Q == 4'b0000 && EN == 1'b1) ? 1'b1 : 1'b0;
end

always @(posedge CLK or posedge RESET) begin
    if (RESET) begin
        Q <= 4'b0000;
    end
    else if (EN) begin
        if (DIR == 1'b1) // Count up
            Q <= Q + 4'b0001;
        else // Count down
            Q <= Q - 4'b0001;
    end
end

endmodule
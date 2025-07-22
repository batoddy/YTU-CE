module shift_register_bh(
    input clk,
    input reset,
    input data_in,
    output reg [3:0] data_out
);
    reg [3:0] register;

    always @(posedge clk or posedge reset) begin
        if (reset)
            register <= 4'b0000;
        else
            // concat LSB 3 bits of reg to data_in value to shift left
            register <= {register[2:0], data_in};
    end
    // register'ın değişen değerini data_out'a assign et
    always @*
        data_out = register;
endmodule
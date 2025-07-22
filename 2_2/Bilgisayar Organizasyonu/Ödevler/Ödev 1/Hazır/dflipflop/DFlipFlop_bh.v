// DFlipFlop_bh.v

/*D tipi flip-flop modülünün tanımını başlatır. 
clk, reset, ve D girişleri ile Q çıkışı tanımlanır.*/

module DFlipFlop_bh(
    input wire clk,
    input wire reset,
    input wire D,
    output reg Q //output "reg" Q, due to dynamic nature of "always block"
); 

/*always @(posedge clk or posedge reset) begin, 
clk'nin yükselen kenarında veya reset'in yükselen kenarında tetiklenen bir bloğun başlangıcını belirtir. 
Bu, flip-flop'un davranışının bu iki sinyale duyarlı olduğunu gösterir.*/

//davranışsal dizayn
//posedge - negedge
always @(posedge clk or posedge reset) begin
    // sıfırlama (reset) aktif olduğunda çıkışı (Q) sıfırlar, aksi halde D girişindeki değeri aynen saklar.
    if (reset)
        Q <= 1'b0; 
    else
        Q <= D;
end

endmodule
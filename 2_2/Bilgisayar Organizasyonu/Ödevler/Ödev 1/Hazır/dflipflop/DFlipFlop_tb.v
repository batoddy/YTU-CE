// DFlipFlop_tb.v
`include "DFlipFlop_bh.v"
`timescale 1ns / 1ns //1 nanosaniye zaman birimi, 1 nanosaniye hassasiyet

module DFlipFlop_tb;

/*
clk (saat), reset (sıfırlama), ve D (veri girişi) kaydedilebilir sinyallerdir, register olarak tanımlanırlar; 
Q ise çıkış telidir (wire).*/
reg clk;
reg reset;
reg D;
wire Q;

/* D flip-flop'u instantiate edilir,
test benchteki sinyaller ilgili modül giriş ve çıkışlarına bağlanır*/
DFlipFlop_bh dff(
    .clk(clk),
    .reset(reset),
    .D(D),
    .Q(Q)
);

/*Bu blokta, CLK sinyali için bir saat dalgası üretilir. 
Her 5 nanosaniyede bir CLK sinyalinin değeri tersine çevrilir, 
bu da 100 MHz'lik bir frekans üretir.*/
initial begin
    clk = 0;
    forever #5 clk = ~clk; // 100MHz clock
end

// Test senaryosu
initial begin
    /* İlk olarak girişler başlatılır, 
    ardından belirli zaman aralıklarında reset ve D sinyalleri değiştirilir. 
    */
    reset = 1; D = 0;
    #10 reset = 0; D = 1;
    #10 D = 0;
    #10 D = 1;
    #20 D = 0;
    #10 $finish; 
end

initial begin
    $dumpfile("DFlipFlop_tb.vcd");
    $dumpvars(0, DFlipFlop_tb);
end

endmodule

//iverilog -o DFlipFlop_tb.vvp DFlipFlop_tb.v
//vvp DFlipFlop_tb.vvp

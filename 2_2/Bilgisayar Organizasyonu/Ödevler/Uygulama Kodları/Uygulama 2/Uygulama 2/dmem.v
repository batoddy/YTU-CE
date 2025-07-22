// File: dmem.v
module dmem(
    input         clk,
    input         we,
    input  [31:0] a,
    input  [31:0] wd,
    output [31:0] rd
);

  // 64 words of 32-bit data memory
  reg [31:0] RAM [0:63];

  // asynchronous read (word-aligned)
  assign rd = RAM[a[31:2]];

  // synchronous write
  always @(posedge clk) begin
    if (we)
      RAM[a[31:2]] <= wd;
  end

endmodule

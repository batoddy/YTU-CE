`timescale 1ns/1ps

module testbench;
  reg         clk;
  reg         reset;
  wire [31:0] WriteData;
  wire [31:0] DataAdr;
  wire        MemWrite;

  // DUT instantiation
  top dut (
    clk,
    reset,
    WriteData,
    DataAdr,
    MemWrite
  );

  // Dump waveforms for GTKWave
  initial begin
    $dumpfile("waveform.vcd");
    $dumpvars(0, testbench);
  end

  // Reset pulse & timeout
  initial begin
    clk   = 0;
    reset = 1;
    #22   reset = 0;
    // if no valid store by 2000 ns, timeout
    #2000;
    $display("** TIMEOUT: no valid store observed **");
    $finish;
  end

  // 100 MHz clock
  always #5 clk = ~clk;

  // Only react to the second store (Addr 100) and ignore Addr 96
  always @(negedge clk) begin
    if (MemWrite) begin
      if (DataAdr === 32'd96) begin
        // First store: ignore
      end
      else if (DataAdr === 32'd100 && WriteData === 32'd25) begin
        $display("Simulation succeeded");
        $finish;
      end
      else begin
        $display("Simulation failed: wrote %0d at addr %0d",
                 WriteData, DataAdr);
        $finish;
      end
    end
  end

endmodule

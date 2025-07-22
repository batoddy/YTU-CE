
`include "counter.v"
module counter_tb;

    
    // Inputs
    reg CLK, RESET, EN;
    
    // Outputs
    wire [3:0] Q;
    wire CO;

    counter uut (
        .CLK(CLK),
        .RESET(RESET),
        .EN(EN),
        .Q(Q),
        .CO(CO)
    );
    
    initial begin
        
        $dumpfile("counter_tb.vcd");
        $dumpvars(0, counter_tb);
        
        RESET = 1'b0;
        EN = 1'b1;
        CLK= 1'b0;
    
        #30; 
        
        // Reset
        RESET = 1'b1;
        #10; 
        RESET = 1'b0;
        #50; 
        
        RESET = 1'b0;
        EN = 1'b0; // Disable counter
    
        #30;
  
        $finish;
    end
    always #5 CLK=~CLK;
endmodule

// iverilog -o counter_tb.vvp counter_tb.v
// vvp counter_tb.vvp
// gtkwave

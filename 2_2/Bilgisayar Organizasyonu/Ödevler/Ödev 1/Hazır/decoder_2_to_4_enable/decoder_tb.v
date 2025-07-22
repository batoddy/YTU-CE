

`include "decoder_df.v"

module decoder_tb;
    
    // Inputs
    reg EN, A0, A1;
    
    // Outputs
    wire D0, D1, D2, D3;
    
    // Instantiate the module under test
    decoder_df uut (
        .EN(EN),
        .A0(A0),
        .A1(A1),
        .D0(D0),
        .D1(D1),
        .D2(D2),
        .D3(D3)
    );
    
    // Clock generation
    initial begin
        EN = 1'b0;
        A0 = 1'b0;
        A1 = 1'b0;
        // Test case 1
        $display("Test Case 1: EN = 0, A0 = 0, A1 = 0");
        #5;
        // Test case 2
        $display("Test Case 2: EN = 1, A0 = 0, A1 = 0");
        EN = 1'b1;
        #5;
        // Test case 3
        $display("Test Case 3: EN = 1, A0 = 1, A1 = 0");
        A0 = 1'b1;
        #5;
        // Test case 4
        $display("Test Case 4: EN = 1, A0 = 0, A1 = 1");
        A0 = 1'b0;
        A1 = 1'b1;
        #5;
        // Test case 5
        $display("Test Case 5: EN = 1, A0 = 1, A1 = 1");
        A0 = 1'b1;
        #5;
    end

    initial begin
        $dumpfile("decoder_tb.vcd");
        $dumpvars(0,decoder_tb);
    end    

endmodule


//iverilog -o decoder_tb.vvp decoder_tb.v
//vvp decoder_tb.vvp
//gtkwave
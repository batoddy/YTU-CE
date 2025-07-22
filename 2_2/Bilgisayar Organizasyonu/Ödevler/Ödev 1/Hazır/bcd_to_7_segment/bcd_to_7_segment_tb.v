`include "bcd_to_7_segment.v"

module bcd_to_7_segment_tb;

    reg [3:0] bcd;
    wire [6:0] seg;
    integer i;

    bcd_to_7_segment uut (
        .bcd(bcd), 
        .seg(seg)
    );

    initial begin
        for(i = 0;i < 16;i = i+1) //run loop for 0 to 15.
        begin
            bcd = i; 
            #10;
        end     
    end
        
    initial begin
        $dumpfile("bcd_to_7_segment_tb.vcd");
        $dumpvars(0, bcd_to_7_segment_tb);
    end

endmodule

//iverilog -o bcd_to_7_segment_tb.vvp bcd_to_7_segment_tb.v
//vvp bcd_to_7_segment_tb.vvp     
//gtkwave
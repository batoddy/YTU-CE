// tb_mealy_fsm.v
module tb_mealy_fsm;
    reg clk, reset, in;
    wire out;

    mealy_fsm uut (
        .clk(clk),
        .reset(reset),
        .in(in),
        .out(out)
    );

    // Clock üretimi
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        $display("Mealy FSM Test Start");
        reset = 1; in = 0; #12; // Reset aktif
        reset = 0;

        // Basit giriş örüntüsü: 1,0,1,1,0
        in = 1; #10;  // '1'
        in = 0; #10;  // '10'
        in = 1; #10;  // '101' anında out=1 (mealy farkı)
        in = 1; #10;  // tekrar '1'
        in = 0; #10;  // ...
        $finish;
    end

    initial begin
        $monitor("Time=%0t: reset=%b, in=%b, out=%b", $time, reset, in, out);
    end
endmodule

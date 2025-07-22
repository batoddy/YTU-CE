// tb_moore_fsm.v
module tb_moore_fsm;
    reg clk, reset, in;
    wire out;

    // DUT (Device Under Test)
    moore_fsm uut (
        .clk(clk),
        .reset(reset),
        .in(in),
        .out(out)
    );

    // Clock üretimi
    initial begin
        clk = 0;
        forever #5 clk = ~clk; // 10 zaman birimi periyot
    end

    initial begin
        $display("Moore FSM Test Start");
        reset = 1; in = 0; #12; // Reset aktif
        reset = 0;

        // Giriş örüntüsü: 1,0,1,1,0,1,0,...
        // '101' sekansını birkaç kez deneyelim
        in = 1; #10; // '1'
        in = 0; #10; // '10'
        in = 1; #10; // '101' => out=1 beklenir
        in = 1; #10; // tekrar '1'
        in = 0; #10; // '10'
        in = 1; #10; // '101' => out=1
        in = 0; #10;

        $finish;
    end

    initial begin
        $monitor("Time=%0t: reset=%b, in=%b, out=%b", $time, reset, in, out);
    end
endmodule

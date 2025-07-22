// moore_fsm.v
module moore_fsm (clk, reset, in, out);
    input clk;       // Clock sinyali
    input reset;     // Asenkron reset
    input in;        // Giriş (bit)
    output out;      // Çıkış (Moore: mevcut duruma göre belirlenir)

    reg [1:0] state;        // Mevcut durum
    reg [1:0] next_state;   // Gelecek durum
    reg out;                // Moore FSM'de çıkış, durum tabanlı bir register

    // Durum tanımlamaları (parametreler)
    parameter S0 = 2'b00;  // Başlangıç durumu
    parameter S1 = 2'b01;
    parameter S2 = 2'b10;

    // Durum geçişi: asenkron reset
    always @(posedge clk or posedge reset) begin
        if (reset)
            state <= S0;
        else
            state <= next_state;
    end

    // Gelecek durum ve çıkış hesaplaması (Moore: çıkış sadece duruma bağlı)
    always @(state or in) begin
        // Varsayılan atamalar
        next_state = state;
        out = 1'b0;

        case (state)
            S0: begin
                // Henüz '101' sekansının hiçbir bitini tespit etmedik
                // Giriş '1' ise kısmen '1' yakaladık, S1'e geç
                // Giriş '0' ise S0'da kal
                if (in)
                    next_state = S1;
                else
                    next_state = S0;
                // out = 0 (Moore FSM'de bu durumda sekans tamamlanmadı)
            end

            S1: begin
                // Bir '1' tespit ettik, sırada '0' bekleniyor
                if (!in)
                    next_state = S2;  // '10' yakaladık
                else
                    next_state = S1;  // Tekrar '1' geldi
            end

            S2: begin
                // '10' yakaladık, şimdi '1' bekleniyor
                if (in) begin
                    // '101' sekansı tamamlandı
                    next_state = S1;  // '101' sonrası tekrar '1' ile başlayabilir
                    out = 1'b1;       // Moore tipi: bu durumdayken out=1
                end
                else
                    next_state = S0;  // '100' oldu, sekans bozuldu
            end

            default: begin
                next_state = S0;
                out = 1'b0;
            end
        endcase
    end
endmodule

// mealy_fsm.v
module mealy_fsm (clk, reset, in, out);
    input clk;       // Clock sinyali
    input reset;     // Asenkron reset
    input in;        // Giriş (bit)
    output out;      // Çıkış (Mealy: durum + girişe bağlı)

    reg state;            // Mevcut durum (1 bit)
    reg next_state;       // Gelecek durum
    reg out;              // Mealy tipi: out her zaman duruma ve girişe göre

    // Durum parametreleri
    parameter S0 = 1'b0;  // Başlangıç durumu
    parameter S1 = 1'b1;

    // Durum geçişi: asenkron reset
    always @(posedge clk or posedge reset) begin
        if (reset)
            state <= S0;
        else
            state <= next_state;
    end

    // Gelecek durum ve çıkış mantığı (Mealy: out hem state hem de in'e bağlı)
    always @(state or in) begin
        // Varsayılan atamalar
        next_state = state;
        out = 1'b0;

        case (state)
            S0: begin
                // S0'dan S1'e geçmek için '1' bekliyoruz
                // '1' gelince hem next_state=S1 hem de out=0 (ya da duruma göre)
                if (in) begin
                    next_state = S1;
                    // out=1 yaparsak "1" geldiği an bir sekans tamamlandığını varsayabilirdik
                    // ama "101" sekansı için henüz tam tespit yok, bu sadece '1' yakaladık
                    out = 1'b0;
                end else begin
                    next_state = S0;
                end
            end

            S1: begin
                // Mevcut durumda bir '1' var. '0' bekleniyor
                if (!in) begin
                    // '10' tespit ettik, mealy olduğundan bir sonraki bit '1' ise anında out=1
                    next_state = S0; // '10' sonrası '0''a dönmek (ya da S0) mantıklı
                end else begin
                    // Tekrar '1' geldi
                    next_state = S1;
                end
                // Mealy farkı: out hesaplaması anında
                // Örneğin "101" dizisinde, '10' durumu ve in=1 gelirse out=1
                // Bunu basit tutmak için:
                if (in == 1'b1) begin
                    // eğer "101" sekansının son '1' bitini tespit ettiysek out=1
                    // ama tam diziyi takip için ek durum gerekebilir
                    // Basit bir mealy senaryosu gösterimi:
                    out = 1'b1;  
                end
            end

            default: begin
                next_state = S0;
                out = 1'b0;
            end
        endcase
    end
endmodule

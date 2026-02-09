#include <Wire.h> // I2C için
#include <WiFi.h> // WiFi için
#include <cmath> // Matematiksel fonksiyonlar için
#include "Adafruit_MPU6050.h" // İvme ve jiroskop için
#include "Adafruit_Sensor.h" // Sensör API'si için
#include "MAX30100_PulseOximeter.h" // Nabız/SpO2 sensörü için

// --- Sensör ve Sistem Nesneleri ---
PulseOximeter pox;
Adafruit_MPU6050 mpu;

// --- Wi-Fi Ayarları ---
const char* ssid = "Aferin"; // Kendi Wi-Fi ağ adınız
const char* password = "batu78999"; // Kendi Wi-Fi şifreniz

// --- Nabız Oksimetre Ayarları (MAX30100) ---
const unsigned long SAMPLE_INTERVAL_MS = 20; // 20 ms'de bir pox.update()
const unsigned long PRINT_INTERVAL_MS = 1000; // 1 sn'de bir ekrana yaz
const unsigned long MIN_MEASURE_MS = 20000; // 20 sn sonra stabilite aramaya başla

// Kayan Pencere Filtre Ayarları
const int HR_WINDOW_SIZE = 15; // 15 sn'lik pencere
float hrWindow[HR_WINDOW_SIZE];
int hrCount = 0;
int hrIndex = 0;

float stableHR = 0; // Son stabil kabul edilen BPM
bool hasStableHR = false;

unsigned long lastSampleMs = 0;
unsigned long lastPrintMs = 0;
unsigned long startMs = 0;

// --- Algoritma Eşik Değerleri ---
const float DUSME_IVME_ESIGI = 2.5; // g (Düşme için anlık yüksek ivme/şok eşiği)
const int HAREKETSIZLIK_SURE_SN = 300; // 5 dakika
const int NABIZ_YUKSEK_ESIK = 120; // bpm (Anormal yüksek nabız)
const int NABIZ_DUSUK_ESIK = 40; // bpm (Anormal düşük nabız)
// const int MANUEL_ALARM_PIN = 4; // Manuel acil durum butonu için GPIO pini

// --- Durum Değişkenleri ---
unsigned long sonHareketZamani = 0;
float rate = 0; // Nabız (BPM)
// float spo2 = 0; // Bu kütüphane ile SpO2 değeri stabil alınmadığı için şimdilik devre dışı

// --- Fonksiyon Tanımlamaları ---
void uyariGonder(String mesaj);

void setup() {
    Serial.begin(115200);
    // pinMode(MANUEL_ALARM_PIN, INPUT_PULLUP);
    Wire.begin(21, 22); // ESP32: SDA=21, SCL=22 I2C Başlatılıyor

    // MPU6050 Başlatma
    Serial.println("MPU6050 Başlatılıyor...");
    if (!mpu.begin()) {
        Serial.println("MPU6050 bulunamadı! Bağlantıları kontrol edin.");
        while (1) delay(100);
    }
    Serial.println("MPU6050 Başlatıldı!");
    mpu.setCycleRate(MPU6050_CYCLE_5_HZ); // Düşük güç modu/hızlı okuma ayarı

    // MAX30100 Başlatma
    Serial.println("\nMAX30100 stabil nabiz olcumu basliyor...");
    if (!pox.begin()) {
        Serial.println("MAX30100 BASLATILAMADI! (baglanti/I2C sorunu)");
        while (1) delay(1000);
    }
    Serial.println("MAX30100 basarili sekilde bulundu.");
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    startMs = millis();
   
    // Wi-Fi Bağlantısı
    WiFi.begin(ssid, password);
    Serial.print("Wi-Fi'a bağlanılıyor");
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(500);
        Serial.print(".");
        attempt++;
    }
   
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Bağlandı!");
        Serial.print("IP Adresi: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWi-Fi Bağlantı Hatası! Uyarı sistemi çalışmayacak.");
    }
}

void loop() {
    unsigned long now = millis();

    // 1. MAX30100 Nabız Güncelleme ve Filtreleme
    if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
        lastSampleMs = now;
        pox.update();

        float hr = pox.getHeartRate();

        // Geçerli BPM ise pencereye ekle
        if (hr > 30 && hr < 180) {
            hrWindow[hrIndex] = hr;
            hrIndex = (hrIndex + 1) % HR_WINDOW_SIZE;
            if (hrCount < HR_WINDOW_SIZE) hrCount++;
        }
    }

    // 2. Nabız Hesaplama, Stabilite Kontrolü ve Seri Çıktı
    if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
        lastPrintMs = now;

        float rawHR = pox.getHeartRate();
        float mean = 0.0, stddev = 0.0;
       
        if (hrCount > 0) {
            for (int i = 0; i < hrCount; i++) mean += hrWindow[i];
            mean /= hrCount;

            for (int i = 0; i < hrCount; i++) {
                float d = hrWindow[i] - mean;
                stddev += d * d;
            }
            stddev = sqrt(stddev / hrCount);
        }

        bool stableNow = false;
        // Stabilite Kontrolü: Yeterli süre + yeterli örnek + düşük oynama
        if ( (now - startMs) > MIN_MEASURE_MS && hrCount >= 8 && stddev < 3.0 ) {
            stableHR = mean;
            hasStableHR = true;
            stableNow = true;
            rate = stableHR; // Uyarı sistemi için stabil nabzı kullan
        } else {
            rate = 0.0; // Stabil değilse 0 kabul et (Uyarı sistemini tetiklemez)
        }

        // Seri Çıktı
        Serial.print("RAW: "); Serial.print(rawHR);
        Serial.print(" | MEAN: "); Serial.print(mean);
        Serial.print(" | STD: "); Serial.print(stddev);
        Serial.print(" | STABLE: "); if (hasStableHR) Serial.print(stableHR); else Serial.print(0);
        Serial.print(" | LOCK: "); Serial.println(stableNow ? "YES" : "NO");
    }
   
    // 3. MPU-6050 Verilerini Oku (Düşme & Hareketsizlik)
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
   
    // Toplam İvme (g cinsinden)
    float toplamIvme = sqrt(
        pow(a.acceleration.x, 2) +
        pow(a.acceleration.y, 2) +
        pow(a.acceleration.z, 2)
    ) / 9.81;

    // --- DÜŞME TESPİTİ ---
    if (toplamIvme > DUSME_IVME_ESIGI) {
        // Yüksek şok sonrası hareketsizlik kontrolü
        delay(500);
        mpu.getEvent(&a, &g, &temp);
        float carpmaSonrasiIvme = sqrt(
            pow(a.acceleration.x, 2) +
            pow(a.acceleration.y, 2) +
            pow(a.acceleration.z, 2)
        ) / 9.81;

        if (carpmaSonrasiIvme < 0.2) {
            Serial.println("DÜŞME TESPİT EDİLDİ!");
            uyariGonder("Ani düşme ve sonrasında hareketsizlik tespit edildi!");
            sonHareketZamani = millis();
        } else {
            sonHareketZamani = millis();
        }
    }

    // --- HAREKETSİZLİK TESPİTİ ---
    if (toplamIvme > 0.3) {
        sonHareketZamani = millis(); // Cihaz hareket ediyorsa zamanı sıfırla
    } else {
        if ((millis() - sonHareketZamani) / 1000 > HAREKETSIZLIK_SURE_SN) {
            Serial.println("UZUN SÜRELİ HAREKETSİZLİK TESPİT EDİLDİ");
            uyariGonder("Kişi uzun süredir hareketsiz. Bayılma/felç riski olabilir.");
            sonHareketZamani = millis(); // Uyarıyı bir kez göndermek için zamanı sıfırla
        }
    }
   
    // --- ANORMAL NABIZ TESPİTİ (Stabil Nabız Kullanılıyor) ---
    // Sadece stabil bir nabız değeri varsa kontrol et (rate != 0.0)
    if (rate != 0.0) {
        if (rate > NABIZ_YUKSEK_ESIK) {
            Serial.printf("ANORMAL YÜKSEK NABIZ TESPİT EDİLDİ: %.0f bpm\n", rate);
            uyariGonder("Anormal YÜKSEK nabız değeri tespit edildi! Kardiyak risk.");
        } else if (rate < NABIZ_DUSUK_ESIK) {
            Serial.printf("ANORMAL DÜŞÜK NABIZ TESPİT EDİLDİ: %.0f bpm\n", rate);
            uyariGonder("Anormal DÜŞÜK nabız değeri tespit edildi! Kardiyak risk.");
        }
    }
   
    // Seri Monitor Özet Çıktısı
    Serial.printf("Toplam İvme: %.2f g | Stabil Nabız: %.0f bpm\n", toplamIvme, stableHR);

    delay(100);
}

void uyariGonder(String mesaj) {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("--- Wi-Fi Üzerinden Uyarı Gönderildi (Mobil Uygulama/API): " + mesaj + " ---");
    } else {
        Serial.println("--- UYARI! Wi-Fi Bağlı Değil, Uyarı Gönderilemedi: " + mesaj + " ---");
    }
    // delay(5000); // Uyarı gönderildikten sonra bekleme (opsiyonel)
}

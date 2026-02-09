#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

PulseOximeter pox;

// örnekleme zamanları
uint32_t lastReport = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("MAX30100 testi basliyor...");

  // ESP32 I2C pinleri
  Wire.begin(21, 22);

  // sensörü başlat
  if (!pox.begin()) {
    Serial.println("MAX30100 BASLAMADI! Baglantilari kontrol edin.");
    while (1) delay(1000);
  }

  Serial.println("MAX30100 baslatildi!");
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() {
  // sensörü her döngüde güncelle
  pox.update();

  // her saniye rapor yaz
  if (millis() - lastReport > 1000) {
    lastReport = millis();

    float bpm = pox.getHeartRate();
    float spo2 = 0; // bu kütüphane SPO2'yi stabil vermiyor

    Serial.print("BPM: ");
    Serial.println(bpm);
  }
}

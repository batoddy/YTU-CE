#include "SensorManager.h"
#include "AlertSystem.h"

SensorManager sensors;
AlertSystem alerts;

// Manuel acil durum butonu
const int EMERGENCY_BUTTON_PIN = 4;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n╔═══════════════════════════════════════╗");
    Serial.println("║   YASLI TAKIP SISTEMI - v2.0          ║");
    Serial.println("║   Offline Mode - Serial Only          ║");
    Serial.println("╚═══════════════════════════════════════╝\n");
    
    // ========== SENSÖR BAŞLATMA ==========
    SensorManager::Config sensorConfig;
    sensorConfig.debug = true;  // Debug açık
    
    if (!sensors.begin(sensorConfig)) {
        Serial.println("KRITIK: Sensorler baslatilamadi!");
        while(1) delay(1000);
    }
    
    // ========== ALERT SİSTEMİ BAŞLATMA ==========
    AlertSystem::Config alertConfig;
    alertConfig.enableSerial = true;
    alertConfig.alertCooldownMs = 3000; 
    
    alerts.begin(alertConfig);
    
    // ========== ACİL DURUM BUTONU ==========
    pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Acil durum butonu (Pin 4) hazir.\n");
    
    Serial.println(">>> SISTEM HAZIR <<<\n");
}

void loop() {
    // ========== SENSÖR GÜNCELLEMELERİ ==========
    // 🔴 HER TURDA - pox.update()
    sensors.fastUpdate();
    
    // 🔵 ANALİZ - ölçüm + analiz
    sensors.slowUpdate();
  

    
    // ========== DURUM KONTROLLERİ ==========
    
    // 1. Düşme tespiti (en yüksek öncelik)
    if (sensors.isDusmeAlgilandi()) {
        auto data = sensors.getData();
        alerts.sendAlert(
            AlertSystem::ALERT_DUSME,
            "Dusme tespit edildi! Acil yardim gerekli!",
            data.toplamIvme
        );
    }
    
    // 2. Nabız kontrolü
if (sensors.isNabizAnormal()) {
    auto data = sensors.getData();
    float nabiz = data.currentHR;   //  DOĞRU DEĞER

    if (nabiz == 0) {
        alerts.sendAlert(
            AlertSystem::ALERT_NABIZ_DUSUK,
            "Nabiz algilanamiyor! Sensor kaybi veya kritik durum!",
            0
        );
    }
    else if (nabiz < 40.0) {
        alerts.sendAlert(
    AlertSystem::ALERT_NABIZ_DUSUK,
    String("DUSUK NABIZ! BPM: ") + String(nabiz, 1),
    nabiz
);

    }
    else if (nabiz > 120.0) {
        alerts.sendAlert(
    AlertSystem::ALERT_NABIZ_YUKSEK,
    String("YUKSEK NABIZ! BPM: ") + String(nabiz, 1),
    nabiz
);

    }
}

    
    // 3. Hareketsizlik kontrolü
    if (sensors.isHareketsizlikUzun()) {
        alerts.sendAlert(
            AlertSystem::ALERT_HAREKETSIZLIK,
            "5 dakikadir hareket algilanmadi!",
            300
        );
    }
    
    // 4. Manuel acil durum butonu
    int reading = digitalRead(EMERGENCY_BUTTON_PIN);
    
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading == LOW) {
            alerts.handleEmergencyButton();
            delay(1000);  // Tekrar basılmasını önle
        }
    }
    
    lastButtonState = reading;
    
    // ========== PERİYODİK RAPORLAMA ==========
    static unsigned long lastReport = 0;
    if (millis() - lastReport >= 10000) {  // 10 saniye
        lastReport = millis();
        
        sensors.printSensorData();
        alerts.printStatus();
    }
}

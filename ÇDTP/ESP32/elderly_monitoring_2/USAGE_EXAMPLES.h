/**
 * KULLANIM ÖRNEKLERİ VE TEST SENARYOLARI
 * 
 * Bu dosya, sistemi test etmek ve özelleştirmek için
 * örnek kullanımları içerir.
 */

#ifndef USAGE_EXAMPLES_H
#define USAGE_EXAMPLES_H

// ======================================================
// ========= ÖRNEK 1: TEMEL KULLANIM ===================
// ======================================================

void basicUsageExample() {
    /*
    // setup() içinde
    SensorManager sensors;
    AlertSystem alerts;
    
    // Sensörleri varsayılan ayarlarla başlat
    sensors.begin();
    
    // Uyarı sistemini başlat
    AlertSystem::Config alertConfig;
    alertConfig.wifiSSID = "SSID";
    alertConfig.wifiPassword = "PASSWORD";
    alerts.begin(alertConfig);
    
    // loop() içinde
    sensors.update();
    
    if (sensors.isDusmeAlgilandi()) {
        alerts.sendAlert(AlertSystem::ALERT_DUSME, "Dusme!");
    }
    */
}

// ======================================================
// ===== ÖRNEK 2: ÖZELLEŞTİRİLMİŞ EŞIKLER =============
// ======================================================

void customThresholdsExample() {
    /*
    // Sporcu için yüksek nabız eşikleri
    SensorManager::Config sensorConfig;
    sensorConfig.nabizDusukEsik = 50.0;   // Sporcular için daha düşük
    sensorConfig.nabizYuksekEsik = 160.0; // Daha yüksek tolerans
    sensorConfig.dusmeIvmeEsigi = 3.0;    // Daha az hassas
    
    sensors.begin(sensorConfig);
    */
}

// ======================================================
// ====== ÖRNEK 3: OFFLINE MOD (WiFi YOK) ==============
// ======================================================

void offlineModeExample() {
    /*
    AlertSystem::Config alertConfig;
    alertConfig.enableWiFi = false;      // WiFi kapat
    alertConfig.enableSerial = true;     // Sadece Serial
    alertConfig.enableBluetooth = false; // BLE kapalı
    
    alerts.begin(alertConfig);
    
    // Sistem sadece Serial üzerinden uyarı verecek
    */
}

// ======================================================
// ======== ÖRNEK 4: WEBHOOK ENTEGRASYONU ==============
// ======================================================

void webhookIntegrationExample() {
    /*
    AlertSystem::Config alertConfig;
    alertConfig.wifiSSID = "SSID";
    alertConfig.wifiPassword = "PASSWORD";
    
    // Webhook ayarları
    alertConfig.webhookURL = "https://api.example.com/alert";
    alertConfig.apiKey = "sk_live_123456789";
    
    alerts.begin(alertConfig);
    
    // Uyarılar otomatik olarak webhook'a POST edilecek
    // JSON format:
    // {
    //   "type": "DUSME",
    //   "priority": 3,
    //   "message": "Dusme tespit edildi!",
    //   "timestamp": 123456789,
    //   "value": 3.45
    // }
    */
}

// ======================================================
// ========= ÖRNEK 5: GERÇEK ZAMANLI VERİ ==============
// ======================================================

void realTimeDataExample() {
    /*
    // loop() içinde
    sensors.update();
    
    auto data = sensors.getData();
    
    // Sensör verilerine doğrudan erişim
    float ivme = data.toplamIvme;
    float nabiz = data.stableHR;
    bool stabil = data.nabizStabil;
    
    // Mobil uygulamaya gönder (Bluetooth)
    if (stabil) {
        sendToMobileApp(nabiz, ivme);
    }
    */
}

// ======================================================
// ======= ÖRNEK 6: DÜŞÜK GÜÇ MODU =====================
// ======================================================

void lowPowerModeExample() {
    /*
    #include <esp_sleep.h>
    
    void enterLowPower() {
        // Sensörleri uykuya al
        // WiFi'yi kapat
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        
        // CPU frekansını düşür
        setCpuFrequencyMhz(80);  // 240 → 80 MHz
        
        // Light sleep (sensörler çalışmaya devam eder)
        esp_sleep_enable_timer_wakeup(1000000);  // 1 saniye
        esp_light_sleep_start();
    }
    
    void loop() {
        sensors.update();
        
        if (!sensors.isHareketVar()) {
            enterLowPower();  // Hareket yoksa düşük güç
        }
        
        delay(100);
    }
    */
}

// ======================================================
// ========= TEST SENARYOLARI ==========================
// ======================================================

void testScenarios() {
    /*
    ==========================================
    TEST 1: DÜŞME TESPİTİ
    ==========================================
    
    Adımlar:
    1. Sistemi başlat
    2. Cihazı hızlıca salla (>2.5g ivme oluştur)
    3. Hemen yere koy (hareketsiz bırak)
    
    Beklenen:
    - "Yuksek ivme algilandi!" mesajı
    - "DUSME TESPIT EDILDI!" mesajı
    - [KRITIK] uyarısı
    
    ==========================================
    TEST 2: HAREKETSİZLİK
    ==========================================
    
    Adımlar:
    1. Sistemi başlat
    2. Cihazı 5 dakika hareketsiz bırak
    
    Beklenen:
    - "5 dakikadir hareket algilanmadi!" uyarısı
    - [UYARI] seviyesi
    
    Not: Test için süreyi azalt:
    const unsigned long HAREKETSIZLIK_SURE_SN = 30; // 30 saniye
    
    ==========================================
    TEST 3: NABIZ ÖLÇÜMÜ
    ==========================================
    
    Adımlar:
    1. MAX30100'ü parmağına yerleştir
    2. 20 saniye bekle (stabilizasyon)
    3. Nabız değerini kontrol et
    
    Beklenen:
    - "Stabil Nabiz: XX bpm" (60-100 arası normal)
    - "Stabil: EVET" durumu
    
    ==========================================
    TEST 4: YÜKSEK NABIZ UYARISI
    ==========================================
    
    Adımlar:
    1. Tempolu egzersiz yap (merdiven çık)
    2. Hemen MAX30100'e parmağını koy
    
    Beklenen:
    - Nabız >120 bpm
    - "NABIZ_YUKSEK" uyarısı
    
    ==========================================
    TEST 5: MANUEL ACİL DURUM
    ==========================================
    
    Adımlar:
    1. Pin 4'ü GND'ye bağla (buton bas)
    2. Butonu bırak
    
    Beklenen:
    - "MANUEL ACIL DURUM BUTONU BASILDI!"
    - [KRITIK] uyarısı
    - WiFi varsa webhook gönderimi
    
    ==========================================
    TEST 6: WiFi BAĞLANTISI
    ==========================================
    
    Adımlar:
    1. Doğru SSID/Password kullan
    2. Sistemi başlat
    3. Serial'i izle
    
    Beklenen:
    - "WiFi baglandi!"
    - "IP Adresi: 192.168.x.x"
    - "Mod: ONLINE"
    
    WiFi yoksa:
    - "WiFi baglanti zaman asimi!"
    - "Mod: OFFLINE"
    
    ==========================================
    TEST 7: I2C SENSÖR TARAMASI
    ==========================================
    
    Kodu setup()'a ekle:
    
    void scanI2C() {
        Serial.println("\nI2C tarama basladi...");
        byte count = 0;
        
        for (byte i = 1; i < 127; i++) {
            Wire.beginTransmission(i);
            if (Wire.endTransmission() == 0) {
                Serial.printf("Adres: 0x%02X\n", i);
                count++;
            }
        }
        
        Serial.printf("Toplam %d cihaz bulundu\n\n", count);
    }
    
    Beklenen cihazlar:
    - 0x68: MPU6050
    - 0x57: MAX30100
    
    ==========================================
    */
}

// ======================================================
// ========= HATA AYIKLAMA İPUÇLARI ====================
// ======================================================

void debuggingTips() {
    /*
    SORUN: MAX30100 bulunamıyor
    ÇÖZÜM:
    - VCC → 3.3V (5V DEĞİL!)
    - Parmağı doğru yerleştir
    - Kütüphane versiyonunu kontrol et
    
    ==========================================
    
    SORUN: MPU6050 veri okumuyor
    ÇÖZÜM:
    - I2C adresini kontrol et (0x68 veya 0x69)
    - AD0 pinini kontrol et
    - Pull-up dirençleri ekle (4.7kΩ)
    
    ==========================================
    
    SORUN: Nabız okuması kararsız
    ÇÖZÜM:
    - Parmağı 20 saniye hareketsiz tut
    - Işık kaynağından uzak dur
    - hrWindowSize artır (15 → 20)
    - hrStabilityThreshold artır (3.0 → 5.0)
    
    ==========================================
    
    SORUN: Yanlış düşme alarmı
    ÇÖZÜM:
    - dusmeIvmeEsigi artır (2.5 → 3.0)
    - Doğrulama süresini ayarla
    
    ==========================================
    
    SORUN: WiFi bağlanamıyor
    ÇÖZÜM:
    - SSID/Password kontrol et
    - 2.4GHz ağ kullan (5GHz değil)
    - wifiTimeoutSeconds artır
    - Static IP dene
    
    ==========================================
    */
}

#endif // USAGE_EXAMPLES_H

# Yaşlı Takip Sistemi - Giyilebilir Sağlık Monitörü

ESP32 tabanlı giyilebilir sağlık takip sistemi. Düşme tespiti, nabız monitörü ve hareketsizlik algılama özellikleri içerir.

## 🎯 Özellikler

### Sensörler
- **MPU6050**: İvmeölçer + Jiroskop (düşme ve hareket tespiti)
- **MAX30100**: Nabız ve SpO2 sensörü
- **Manuel Acil Durum Butonu**: Pin 4

### Tespit Senaryoları

#### 1. Düşme Tespiti
- Ani yüksek ivme (>2.5g) algılandığında
- Ardından hareketsizlik (<0.2g) kontrolü
- Otomatik acil uyarı gönderimi

#### 2. Uzun Süreli Hareketsizlik
- 5 dakika boyunca hareket yoksa uyarı
- Bayılma veya felç olasılığı
- Orta öncelikli alarm

#### 3. Anormal Nabız
- **Düşük Nabız**: <40 bpm
- **Yüksek Nabız**: >120 bpm
- Stabil ölçüm sonrası uyarı

#### 4. Manuel Acil Durum
- Kullanıcı butona basarak yardım çağırabilir
- En yüksek öncelikli alarm

## 📁 Dosya Yapısı

```
project/
├── main.cpp              # Ana program mantığı
├── SensorManager.h       # Sensör yönetimi (header)
├── SensorManager.cpp     # Sensör yönetimi (implementation)
├── AlertSystem.h         # Uyarı sistemi (header)
├── AlertSystem.cpp       # Uyarı sistemi (implementation)
└── README.md             # Bu dosya
```

## 🔧 Donanım Gereksinimleri

### ESP32 DevKit
- ESP32-WROOM-32
- I2C pinleri: SDA=21, SCL=22

### Sensörler
| Sensör    | Adres  | VCC | GND | SDA | SCL |
|-----------|--------|-----|-----|-----|-----|
| MPU6050   | 0x68   | 3.3V| GND | 21  | 22  |
| MAX30100  | 0x57   | 3.3V| GND | 21  | 22  |

### Diğer
- Acil durum butonu: Pin 4 → GND (internal pullup)

## 📦 Kütüphane Gereksinimleri

```ini
lib_deps = 
    wire
    wifi
    httpclient
    oxullo/MAX30100lib @ ^1.2.1
```

## ⚙️ Yapılandırma

### Sensör Eşikleri (SensorManager::Config)

```cpp
sensorConfig.dusmeIvmeEsigi = 2.5;        // Düşme ivme eşiği (g)
sensorConfig.hareketEsigi = 0.3;          // Hareket eşiği (g)
sensorConfig.nabizDusukEsik = 40.0;       // Düşük nabız (bpm)
sensorConfig.nabizYuksekEsik = 120.0;     // Yüksek nabız (bpm)
```

### WiFi Ayarları (AlertSystem::Config)

```cpp
alertConfig.wifiSSID = "SSID";
alertConfig.wifiPassword = "PASSWORD";
alertConfig.webhookURL = "https://server.com/api/alert";  // Opsiyonel
```

## 🚀 Kurulum

### PlatformIO ile

```bash
# Projeyi klonla
git clone [repo-url]
cd elderly-monitoring

# Derle ve yükle
pio run --target upload

# Serial monitor
pio device monitor
```

### Arduino IDE ile

1. Tüm dosyaları aynı klasöre kopyala
2. `main.cpp` dosyasını `elderly_monitoring.ino` olarak yeniden adlandır
3. Gerekli kütüphaneleri yükle
4. ESP32 kartını seç ve yükle

## 📊 Kullanım

### Başlangıç

```
╔═══════════════════════════════════════╗
║   YASLI TAKIP SISTEMI - v1.0          ║
║   Giyilebilir Saglik Monitoru         ║
╚═══════════════════════════════════════╝

========== SENSOR SETUP ==========
MAX30100 baslatiliyor...
MAX30100 hazir.
MPU6050 baslatiliyor...
MPU6050 hazir.
==================================

========== ALERT SYSTEM ==========
WiFi'ye baglaniyor: Aferin
WiFi baglandi!
IP Adresi: 192.168.1.100
Mod: ONLINE
==================================

>>> SISTEM HAZIR <<<
```

### Periyodik Raporlar (her 10 saniye)

```
========== SENSOR DATA ==========
Ivme (x,y,z): 0.02, -0.01, 0.98 g
Toplam Ivme: 0.98 g
Stabil Nabiz: 72 bpm (Stabil: EVET)
HR Std Dev: 2.1
HR Ornekler: 15/15
================================
```

### Uyarı Örnekleri

```
==================================================
[KRITIK] DUSME: Dusme tespit edildi!
Zaman: 123456 ms
Deger: 3.45
==================================================

==================================================
[ACIL] NABIZ_YUKSEK: Nabiz cok yuksek: 135 bpm
Zaman: 234567 ms
Deger: 135.00
==================================================
```

## 🔋 Enerji Verimliliği

### Mevcut Optimizasyonlar
- 200ms döngü gecikmesi
- Sensör örnekleme aralıkları optimize edilmiş
- WiFi opsiyonel (offline mod destekli)

### Gelecek İyileştirmeler
```cpp
// Deep sleep modu
esp_sleep_enable_timer_wakeup(30 * 1000000);  // 30 saniye
esp_deep_sleep_start();

// Dinamik frekans ayarı
setCpuFrequencyMhz(80);  // 240MHz → 80MHz
```

## 📱 Mobil Uygulama Entegrasyonu

### Webhook API Format

```json
POST /api/alert
{
  "type": "DUSME",
  "priority": 3,
  "message": "Dusme tespit edildi!",
  "timestamp": 123456789,
  "value": 3.45
}
```

### Bluetooth (Gelecek)
- BLE GATT server
- Gerçek zamanlı sensör verisi akışı
- Düşük güç tüketimi

## 🛠️ Özelleştirme

### Hareketsizlik Süresini Değiştir

```cpp
// main.cpp içinde
const unsigned long HAREKETSIZLIK_SURE_SN = 300;  // 5 dakika → 10 dakika
```

### Nabız Eşiklerini Ayarla

```cpp
// setup() içinde
sensorConfig.nabizDusukEsik = 50.0;   // 40 → 50 bpm
sensorConfig.nabizYuksekEsik = 140.0; // 120 → 140 bpm
```

### Webhook Ekle

```cpp
alertConfig.webhookURL = "https://your-server.com/alert";
alertConfig.apiKey = "your-secret-key";
```

## 🐛 Hata Ayıklama

### Sensör Bağlantı Sorunları

```cpp
// I2C tarama ekle
void scanI2C() {
    for (byte i = 0; i < 127; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.printf("I2C cihaz bulundu: 0x%02X\n", i);
        }
    }
}
```

### WiFi Bağlantı Sorunları

```cpp
// Serial'de WiFi durumu
Serial.println(WiFi.status());
Serial.println(WiFi.SSID());
```

## 📈 Performans

- **Loop süresi**: ~50-100ms (sensör okumalarıyla)
- **Bellek kullanımı**: ~30KB RAM
- **WiFi latency**: ~200-500ms (webhook gönderimi)

## 🔐 Güvenlik

- API anahtarı kullanımı (opsiyonel)
- HTTPS webhook desteği
- Veri şifreleme (gelecek)

## 📝 Lisans

MIT License - Eğitim amaçlı kullanım için serbesttir.

## 👥 Katkıda Bulunanlar

- Geliştirici: [Adınız]
- Proje: Yaşlı Sağlık Takip Sistemi
- Platform: ESP32 + Arduino Framework

## 🔗 Kaynaklar

- [ESP32 Datasheet](https://www.espressif.com/en/products/socs/esp32)
- [MPU6050 Library](https://github.com/jrowberg/i2cdevlib)
- [MAX30100 Library](https://github.com/oxullo/Arduino-MAX30100)

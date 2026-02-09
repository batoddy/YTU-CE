#include <Wire.h>
#include <cmath>

// --- MPU6050 I2C adresi ---
const uint8_t MPU_ADDR = 0x68;

// --- Algoritma Eşik Değerleri ---
const float DUSME_IVME_ESIGI      = 2.5;  // g (Düşme için anlık yüksek ivme/şok eşiği)
const int   HAREKETSIZLIK_SURE_SN = 300;  // 5 dakika

// --- Durum Değişkenleri ---
unsigned long sonHareketZamani = 0;

// Basit uyarı fonksiyonu (şimdilik sadece Serial’e yazıyor)
void uyariGonder(const String &mesaj) {
  Serial.println("--- UYARI: " + mesaj + " ---");
}

// MPU başlatma
bool mpuInit() {
  // PWR_MGMT_1 = 0x6B register'ına 0 yazarak cihazı uyandır
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  if (Wire.endTransmission(true) != 0) {
    return false;
  }

  // WHO_AM_I (0x75) kontrolü (beklenen 0x68)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)1, (uint8_t)true);

  if (Wire.available()) {
    uint8_t whoami = Wire.read();
    Serial.print("WHO_AM_I: 0x");
    Serial.println(whoami, HEX);
    if (whoami != 0x68) {
      Serial.println("Beklenen WHO_AM_I 0x68 degil, ama devam etmeyi deneyecegiz.");
    }
    return true;
  } else {
    Serial.println("WHO_AM_I okunamadi!");
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ESP32 I2C pinleri: SDA = 21, SCL = 22
  Wire.begin(21, 22);
  Wire.setClock(100000); // 100 kHz

  Serial.println("MPU6050 (Wire) ile baslatiliyor...");

  if (!mpuInit()) {
    Serial.println("MPU6050 baslatilamadi! Baglantilari kontrol edin.");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MPU6050 basariyla baslatildi!");
  sonHareketZamani = millis();
}

void loop() {
  // 0x3B adresinden itibaren 14 byte oku:
  // Accel X,Y,Z (6), Temp (2), Gyro X,Y,Z (6)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) {
    Serial.println("MPU6050'den veri baslatma hatasi!");
    delay(500);
    return;
  }

  uint8_t n = Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true);
  if (n != 14) {
    Serial.print("Beklenen 14 byte gelmedi, gelen: ");
    Serial.println(n);
    delay(200);
    return;
  }

  int16_t AcX = (Wire.read() << 8) | Wire.read();
  int16_t AcY = (Wire.read() << 8) | Wire.read();
  int16_t AcZ = (Wire.read() << 8) | Wire.read();
  int16_t Tmp = (Wire.read() << 8) | Wire.read(); // kullanmiyoruz ama okuyoruz
  int16_t GyX = (Wire.read() << 8) | Wire.read();
  int16_t GyY = (Wire.read() << 8) | Wire.read();
  int16_t GyZ = (Wire.read() << 8) | Wire.read();

  // Yaklaşık dönüşümler:
  float Ax = AcX / 16384.0; // g
  float Ay = AcY / 16384.0; // g
  float Az = AcZ / 16384.0; // g

  float Gx = GyX / 131.0;   // deg/s
  float Gy = GyY / 131.0;
  float Gz = GyZ / 131.0;

  // Toplam ivme (senin kullandığın gibi)
  float toplamIvme = sqrt(
    pow(Ax, 2) +
    pow(Ay, 2) +
    pow(Az, 2)
  );

  // Seri çıktılar (debug)
  Serial.print("Accel[g] X: "); Serial.print(Ax, 2);
  Serial.print("  Y: ");        Serial.print(Ay, 2);
  Serial.print("  Z: ");        Serial.print(Az, 2);

  Serial.print("  |  Gyro[deg/s] X: "); Serial.print(Gx, 1);
  Serial.print("  Y: ");               Serial.print(Gy, 1);
  Serial.print("  Z: ");               Serial.print(Gz, 1);

  Serial.print("  |  Toplam Ivme: ");  Serial.print(toplamIvme, 2);
  Serial.println(" g");

  // --- DÜŞME TESPİTİ (senin mantığınla benzer) ---
  if (toplamIvme > DUSME_IVME_ESIGI) {
    // Yüksek şok sonrası kısa bekle ve tekrar ölç
    delay(500);

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, (uint8_t)6, (uint8_t)true);
    if (Wire.available() == 6) {
      int16_t AcX2 = (Wire.read() << 8) | Wire.read();
      int16_t AcY2 = (Wire.read() << 8) | Wire.read();
      int16_t AcZ2 = (Wire.read() << 8) | Wire.read();

      float Ax2 = AcX2 / 16384.0;
      float Ay2 = AcY2 / 16384.0;
      float Az2 = AcZ2 / 16384.0;

      float carpmaSonrasiIvme = sqrt(Ax2*Ax2 + Ay2*Ay2 + Az2*Az2);

      if (carpmaSonrasiIvme < 0.2) {
        Serial.println("DUSME TESPIT EDILDI!");
        uyariGonder("Ani dusme ve sonrasinda hareketsizlik tespit edildi!");
        sonHareketZamani = millis();
      } else {
        sonHareketZamani = millis();
      }
    }
  }

  // --- HAREKETSİZLİK TESPİTİ ---
  if (toplamIvme > 0.3) {
    sonHareketZamani = millis(); // Cihaz hareket ediyorsa zamanı sıfırla
  } else {
    unsigned long gecenSn = (millis() - sonHareketZamani) / 1000;
    if (gecenSn > HAREKETSIZLIK_SURE_SN) {
      Serial.println("UZUN SURELI HAREKETSIZLIK TESPIT EDILDI");
      uyariGonder("Kisi uzun suredir hareketsiz. Bayilma/felc riski olabilir.");
      sonHareketZamani = millis(); // Uyarıyı bir kez göndermek için zamanı sıfırla
    }
  }

  delay(200);
}

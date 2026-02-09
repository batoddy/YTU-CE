#include "SensorManager.h"

SensorManager::SensorManager()
    : lastPrintMs(0), startMs(0) {
    memset(&data, 0, sizeof(data));
}

bool SensorManager::begin(const Config& cfg) {
    config = cfg;

    Wire.begin(config.sda, config.scl);

    /* ================= MPU6050 ================= */
    Wire.beginTransmission(MPU_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("MPU6050 bulunamadi!");
        data.mpuReady = false;
    } else {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6B);
        Wire.write(0x00);
        Wire.endTransmission(true);
        data.mpuReady = true;
        data.sonHareketZamani = millis();
        Serial.println("MPU6050 hazir.");
    }

    /* ================= MAX30100 ================= */
    if (!pox.begin()) {
        Serial.println("MAX30100 baslatilamadi!");
        data.maxReady = false;
        return false;
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    data.maxReady = true;

    Serial.println("MAX30100 hazir. Parmaginizi sensore koyun.");
    startMs = millis();
    return true;
}

/* =================================================
   🔴 FAST UPDATE (HER LOOP)
   ================================================= */
void SensorManager::fastUpdate() {
    if (data.maxReady)
        pox.update();
}

/* =================================================
   🔵 SLOW UPDATE
   ================================================= */
void SensorManager::slowUpdate() {
    unsigned long now = millis();

    /* ---------- MPU ---------- */
    if (data.mpuReady)
        readMPU();

    /* ---------- MAX ---------- */
    if (data.maxReady && now - lastPrintMs >= config.printInterval) {
        lastPrintMs = now;

        float hr = pox.getHeartRate();
        data.currentHR = hr;

        if (hr >= config.nabizMin &&
            hr <= config.nabizMax &&
            now - startMs >= config.minMeasureTime) {
            data.stableHR = hr;
            data.nabizStabil = true;
        } else {
            data.nabizStabil = false;
        }

        if (config.debug) {
            Serial.print("[MAX] BPM: ");
            Serial.print(hr);
            Serial.print(" | Stabil: ");
            Serial.println(data.nabizStabil ? "EVET" : "HAYIR");
            
            Serial.print("[MPU] ax: ");
            Serial.print(data.ax, 2);
            Serial.print(" ay: ");
            Serial.print(data.ay, 2);
            Serial.print(" az: ");
            Serial.print(data.az, 2);
            Serial.print(" | Net: ");
            Serial.print(data.toplamIvme, 2);
            Serial.println(" g");

        }
    }
}

/* ================= MPU OKUMA ================= */
bool SensorManager::readMPU() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    if (Wire.endTransmission(false) != 0) return false;

    Wire.requestFrom(MPU_ADDR, (uint8_t)6, (uint8_t)true);
    if (Wire.available() < 6) return false;

    int16_t AcX = (Wire.read() << 8) | Wire.read();
    int16_t AcY = (Wire.read() << 8) | Wire.read();
    int16_t AcZ = (Wire.read() << 8) | Wire.read();

    data.ax = AcX / 16384.0;
    data.ay = AcY / 16384.0;
    data.az = AcZ / 16384.0;

    float total = sqrt(data.ax * data.ax +
                       data.ay * data.ay +
                       data.az * data.az);

    data.toplamIvme = fabs(total - 1.0);

    if (data.toplamIvme > config.hareketEsigi)
        data.sonHareketZamani = millis();



    return true;
}

/* ================= DURUM KONTROLLERİ ================= */

bool SensorManager::isDusmeAlgilandi() {

    // 1️⃣ Ani ivme yoksa çık
    if (data.toplamIvme <= config.dusmeIvmeEsigi)
        return false;

    Serial.println("[DUSME] Ani ivme algilandi, dogrulama basliyor...");

    // 2️⃣ 2 saniye bekle
    unsigned long waitStart = millis();
    while (millis() - waitStart < 2000) {
        fastUpdate();
        readMPU();
        delay(10);
    }

    // 3️⃣ 5 saniye hareketsizlik kontrolü
    Serial.println("[DUSME] Hareketsizlik kontrolu (5 sn)");
    unsigned long checkStart = millis();

    while (millis() - checkStart < 5000) {
        fastUpdate();
        readMPU();

        // Hareket varsa düşme iptal
        if (data.toplamIvme > config.hareketEsigi) {
            Serial.println("[DUSME] Hareket algilandi, dusme IPTAL");
            data.sonHareketZamani = millis();
            return false;
        }

        delay(10);
    }

    // 4️⃣ 5 saniye boyunca HİÇ hareket yok
    Serial.println(">>> DUSME TESPIT EDILDI! <<<");
    data.sonHareketZamani = millis();
    return true;
}


bool SensorManager::isHareketsizlikUzun() {
    unsigned long gecen =
        (millis() - data.sonHareketZamani) / 1000;
    return gecen > config.hareketsizlikSureSn;
}

bool SensorManager::isNabizAnormal() const {
    float hr = data.currentHR;

    // 1) Okuma yok / parmak yok / sinyal yok
    if (hr <= 0.0f) {
        return true; // istersen ayrı alert tipine çevirebiliriz
    }

    // 2) Çok uç değerler -> büyük ihtimal noise (PPG sensörlerde sık)
    // Burayı parametre yapacağız (aşağıda).
    if (hr > config.maxPhysicalBpm) {
        return false; // noise say, alarm verme
    }

    // 3) Stabiliteye BAKMADAN klinik eşikler
    if (hr < config.lowBpmThreshold || hr > config.highBpmThreshold) {
        return true;
    }

    return false;
}




void SensorManager::printSensorData() const {
    Serial.println("====== SENSOR DATA ======");
    Serial.printf("Ivme: %.2f g\n", data.toplamIvme);
    Serial.printf("HR: %.0f | Stabil: %s\n",
                  data.stableHR,
                  data.nabizStabil ? "EVET" : "HAYIR");
    Serial.println("=========================");
}

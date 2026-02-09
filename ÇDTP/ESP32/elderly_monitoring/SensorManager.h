#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <cmath>
#include "MAX30100_PulseOximeter.h"

class SensorManager {
public:
    struct Config {
    /* ================= I2C ================= */
    int sda;
    int scl;

    /* ================= MPU ================= */
    float dusmeIvmeEsigi;          // Ani ivme eşiği (g)
    float hareketEsigi;            // Hareket algılama eşiği
    unsigned long hareketsizlikSureSn; // sn

    /* ================= MAX30100 ================= */
    unsigned long printInterval;   // ms
    unsigned long minMeasureTime;  // ms
    float nabizMin;                // ölçüm geçerli alt sınır
    float nabizMax;                // ölçüm geçerli üst sınır

    /* ================= NABIZ ALARM ================= */
    float lowBpmThreshold;         // Klinik düşük nabız
    float highBpmThreshold;        // Klinik yüksek nabız
    float maxPhysicalBpm;          // Noise filtresi (PPG için)

    /* ================= GENEL ================= */
    bool debug;

    /* ================= CONSTRUCTOR ================= */
    Config() {
        // I2C
        sda = 21;
        scl = 22;

        // MPU
        dusmeIvmeEsigi = 1.5;      // ani ivme
        hareketEsigi = 0.3;        // hareket algılama
        hareketsizlikSureSn = 300; // 5 dk

        // MAX30100
        printInterval = 1000;      // 1 sn
        minMeasureTime = 5000;     // 5 sn
        nabizMin = 30;
        nabizMax = 180;

        // Nabız alarm eşikleri
        lowBpmThreshold  = 40.0;
        highBpmThreshold = 120.0;
        maxPhysicalBpm   = 220.0;

        debug = true;
    }
};


    struct SensorData {
        // MPU
        float ax, ay, az;
        float toplamIvme;
        unsigned long sonHareketZamani;
        
        // MAX
        float currentHR;
        float stableHR;
        bool nabizStabil;
        
        bool mpuReady;
        bool maxReady;
    };

    SensorManager();
    bool begin(const Config& cfg = Config());

    // 🔴 FAST (loop içinde sürekli)
    void fastUpdate();   // pox.update()

    // 🔵 SLOW (zaman kontrollü)
    void slowUpdate();   // ölçüm + analiz

    // Durum kontrolleri
    bool isDusmeAlgilandi();
    bool isHareketsizlikUzun();
    bool isNabizAnormal() const;

    SensorData getData() const { return data; }
    void printSensorData() const;

private:
    // MPU
    static const uint8_t MPU_ADDR = 0x68;
    bool readMPU();

    // MAX
    PulseOximeter pox;

    Config config;
    SensorData data;
    unsigned long lastPrintMs;
    unsigned long startMs;
};

#endif

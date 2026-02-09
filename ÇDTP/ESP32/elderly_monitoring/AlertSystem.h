#ifndef ALERT_SYSTEM_H
#define ALERT_SYSTEM_H

#include <Arduino.h>

class AlertSystem {
public:
    enum AlertType {
        ALERT_DUSME,
        ALERT_HAREKETSIZLIK,
        ALERT_NABIZ_DUSUK,
        ALERT_NABIZ_YUKSEK,
        ALERT_MANUEL,
        ALERT_SISTEM
    };

    enum AlertPriority {
        PRIORITY_LOW,
        PRIORITY_MEDIUM,
        PRIORITY_HIGH,
        PRIORITY_CRITICAL
    };

    enum SensorSource {
        SRC_MPU,
        SRC_PPG,
        SRC_BUTTON,
        SRC_SYSTEM
    };

    struct Config {
        bool enableSerial;
        bool enableBluetooth;
        bool enableGSM;
        unsigned long alertCooldownMs;

        Config() {
            enableSerial = true;
            enableBluetooth = false;
            enableGSM = false;
            alertCooldownMs = 60000;
        }
    };

    struct AlertInfo {
        AlertType type;
        AlertPriority priority;
        SensorSource source;
        String message;
        unsigned long timestamp;
        float sensorValue;
    };

    AlertSystem();

    bool begin(const Config& cfg);

    // ✅ YENİ: Kaynak belirterek (istersen kullanırsın)
    void sendAlert(AlertType type,
                   SensorSource source,
                   const String& message,
                   float value = 0);

    // ✅ ESKİ: Senin mevcut .ino çağrıların için overload (kırılma yok)
    void sendAlert(AlertType type,
                   const String& message,
                   float value = 0);

    void handleEmergencyButton();

    void printStatus() const;

private:
    Config config;
    AlertInfo lastAlert;
    unsigned long lastAlertTime[6];

    bool canSendAlert(AlertType type);
    AlertPriority getPriorityForType(AlertType type);

    // ✅ static yaptık -> const hatası biter
    static String getAlertTypeName(AlertType type);
    static String getSourceName(SensorSource src);

    // Eski API çağrılarında otomatik kaynak seçimi
    static SensorSource inferSourceFromType(AlertType type);

    void sendSerialAlert(const AlertInfo& alert);

    void dispatchAlert(const AlertInfo& alert);

    void handleDusmeAlert(const AlertInfo& alert);
    void handleNabizDusukAlert(const AlertInfo& alert);
    void handleNabizYuksekAlert(const AlertInfo& alert);
    void handleHareketsizlikAlert(const AlertInfo& alert);
    void handleManuelAlert(const AlertInfo& alert);
    void handleSistemAlert(const AlertInfo& alert);


};

#endif

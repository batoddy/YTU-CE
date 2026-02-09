#include "AlertSystem.h"

AlertSystem::AlertSystem() {
    for (int i = 0; i < 6; i++) lastAlertTime[i] = 0;

    lastAlert.type = ALERT_SISTEM;
    lastAlert.priority = PRIORITY_LOW;
    lastAlert.source = SRC_SYSTEM;
    lastAlert.message = "Sistem basladi";
    lastAlert.timestamp = 0;
    lastAlert.sensorValue = 0;
}

bool AlertSystem::begin(const Config& cfg) {
    config = cfg;

    Serial.println("\n========== ALERT SYSTEM ==========");
    Serial.println("Mod: OFFLINE");
    Serial.println("Kanallar:");
    Serial.printf("- Serial     : %s\n", config.enableSerial ? "ACIK" : "KAPALI");
    Serial.printf("- Bluetooth  : %s\n", config.enableBluetooth ? "HAZIR" : "KAPALI");
    Serial.printf("- GSM        : %s\n", config.enableGSM ? "HAZIR" : "KAPALI");
    Serial.println("==================================\n");

    return true;
}

/* =========================================================
   ✅ BACKWARD COMPATIBLE: eski imza
   alerts.sendAlert(ALERT_X, "mesaj", value);
   ========================================================= */
void AlertSystem::sendAlert(AlertType type,
                            const String& message,
                            float value) {
    SensorSource src = inferSourceFromType(type);
    sendAlert(type, src, message, value);z
}

/* =========================================================
   ✅ NEW: kaynak belirterek
   alerts.sendAlert(ALERT_X, SRC_MPU, "mesaj", value);
   ========================================================= */
void AlertSystem::sendAlert(AlertType type,
                            SensorSource source,
                            const String& message,
                            float value) {
    if (!canSendAlert(type)) {
        if (config.enableSerial) {
           // Serial.println("Uyari cooldown suresi icinde, atlandi...");
        }
        return;
    }

    AlertInfo alert;
    alert.type = type;
    alert.priority = getPriorityForType(type);
    alert.source = source;
    alert.message = message;
    alert.timestamp = millis();
    alert.sensorValue = value;

    lastAlert = alert;
    lastAlertTime[type] = millis();

    if (config.enableSerial) {
        sendSerialAlert(alert);
    }

    // Bluetooth/GSM ileride buraya eklenir (şimdilik offline/serial)
}

void AlertSystem::handleEmergencyButton() {
    sendAlert(ALERT_MANUEL, SRC_BUTTON, "Kullanici manuel acil durum butonuna basti", 0);
}

void AlertSystem::sendSerialAlert(const AlertInfo& alert) {
    String p;
    switch (alert.priority) {
        case PRIORITY_CRITICAL: p = "[KRITIK]"; break;
        case PRIORITY_HIGH:     p = "[ACIL]";   break;
        case PRIORITY_MEDIUM:   p = "[UYARI]";  break;
        default:                p = "[BILGI]";  break;
    }

    Serial.println("\n========================================");
    Serial.printf("%s %s\n", p.c_str(), getAlertTypeName(alert.type).c_str());
    Serial.printf("Mesaj  : %s\n", alert.message.c_str());
    Serial.printf("Kaynak : %s\n", getSourceName(alert.source).c_str());
    Serial.printf("Zaman  : %lu ms\n", alert.timestamp);

    if (alert.sensorValue != 0) {
        Serial.printf("Deger  : %.2f\n", alert.sensorValue);
    }

    Serial.println("========================================\n");
}

bool AlertSystem::canSendAlert(AlertType type) {
    if (type == ALERT_DUSME || type == ALERT_MANUEL) return true;
    return (millis() - lastAlertTime[type]) >= config.alertCooldownMs;
}

AlertSystem::AlertPriority AlertSystem::getPriorityForType(AlertType type) {
    switch (type) {
        case ALERT_DUSME:
        case ALERT_MANUEL:
            return PRIORITY_CRITICAL;
        case ALERT_NABIZ_DUSUK:
        case ALERT_NABIZ_YUKSEK:
            return PRIORITY_HIGH;
        case ALERT_HAREKETSIZLIK:
            return PRIORITY_MEDIUM;
        default:
            return PRIORITY_LOW;
    }
}

AlertSystem::SensorSource AlertSystem::inferSourceFromType(AlertType type) {
    switch (type) {
        case ALERT_DUSME:
        case ALERT_HAREKETSIZLIK:
            return SRC_MPU;
        case ALERT_NABIZ_DUSUK:
        case ALERT_NABIZ_YUKSEK:
            return SRC_PPG;
        case ALERT_MANUEL:
            return SRC_BUTTON;
        default:
            return SRC_SYSTEM;
    }
}

String AlertSystem::getAlertTypeName(AlertType type) {
    switch (type) {
        case ALERT_DUSME:         return "DUSME";
        case ALERT_HAREKETSIZLIK: return "HAREKETSIZLIK";
        case ALERT_NABIZ_DUSUK:   return "NABIZ_DUSUK";
        case ALERT_NABIZ_YUKSEK:  return "NABIZ_YUKSEK";
        case ALERT_MANUEL:        return "MANUEL_ACIL";
        default:                  return "SISTEM";
    }
}

String AlertSystem::getSourceName(SensorSource src) {
    switch (src) {
        case SRC_MPU:    return "IVME SENSORU (MPU)";
        case SRC_PPG:    return "KALP SENSORU (PPG)";
        case SRC_BUTTON: return "BUTON";
        default:         return "SISTEM";
    }
}

void AlertSystem::printStatus() const {
    Serial.println("\n========== ALERT STATUS ==========");
    Serial.printf("Son Uyari: %s\n", lastAlert.message.c_str());
    Serial.printf("Kaynak  : %s\n", getSourceName(lastAlert.source).c_str());
    Serial.printf("Zaman   : %lu ms\n", lastAlert.timestamp);
    Serial.println("=================================\n");
}

#include "MyWiFiManager.h"

void MyWiFiManager::setupWiFi() {
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(180);
    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        Serial.println("Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    } else {
        Serial.print("Connected to WiFi. IP Address: ");
        Serial.println(WiFi.localIP());
    }
}



/*
Quinton Nelson
3/13/2024
This file handles the main loop and setup of the program. It initializes the global objects and handles the main loop of the program.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <base64.h> // Include the base64 library for encoding and decoding

#include "EEPROMLayoutManager.h"
#include "TimeManager.h"
#include "WebPage.h"
#include "RelayManager.h"

// Global objects
EEPROMLayoutManager eepromManager;
const int relayPin = 5;
ESP8266WebServer server(80);
RelayManager relay(relayPin);
TimeManager timeManager;


void setup() {
    Serial.begin(115200); // Start serial communication at 115200 baud
    eepromManager.begin(512); // Initialize EEPROM with 512 bytes

    // Attempt to load settings from EEPROM
    String deviceName = eepromManager.loadDeviceName();
    int ringDuration = eepromManager.loadRingDuration();

    if(deviceName.length() == 0) {
        // No device name saved, use a default name
        deviceName = "bellsystem";
    } else {
        deviceName + "bellsystem"; // Append "bellsystem" to the device name
        Serial.print("Device name loaded: ");
        Serial.println(deviceName);
    }

    // Setup WiFi
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        Serial.println("Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }

    // Setup mDNS
    if (!MDNS.begin(deviceName.c_str())) { 
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
        MDNS.begin("bellsystem"); // Start the mDNS responder with URL bellSystem.local
    }

    // Setup routes
    server.on("/", HTTP_GET, []() {
      Serial.println("Home page called");
      WebPage homePage("Home Page");
      String homePageHtml = homePage.generatePage();
      server.send(200, "text/html", homePageHtml);
    });

    server.on("/Settings", HTTP_GET, []() {
      Serial.println("Settings page called");
      WebPage settingsPage("Settings");
      String settingsPageHtml = settingsPage.generatePage();
      server.send(200, "text/html", settingsPageHtml);
    });

    server.on("/SaveSettings", HTTP_POST, []() {
      Serial.println("SaveSettings called");
      if (server.hasArg("deviceName")) {
            String deviceName = server.arg("deviceName");
            eepromManager.saveDeviceName(deviceName);
            Serial.print("Device name received: ");
            Serial.println(deviceName);
        }
        if (server.hasArg("ringDuration")) {
            int ringDuration = server.arg("ringDuration").toInt();
            eepromManager.saveRingDuration(ringDuration);
            Serial.print("Ring duration received: ");
            Serial.println(ringDuration);
        }
      server.send(200, "text/plain", "Settings saved");
    });

    // Example route for toggling the relay
    server.on("/ToggleRelay", HTTP_GET, []() {
      Serial.println("ToggleRelay called");
      relay.activateRelay(2); // Activate the relay for 2 seconds
      server.send(200, "text/plain", "Relay toggle successful");
    });

    // Handle not found
    server.onNotFound([]() {
        server.send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    MDNS.update();
}




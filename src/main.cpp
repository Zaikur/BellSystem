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
#include <EEPROM.h> // Include the EEPROM library for reading and writing to non-volatile memory

#include <base64.h> // Include the base64 library for encoding and decoding

#include "TimeManager.h"
#include "WebPage.h"
#include "RelayManager.h"

// Global objects
const int relayPin = 5;
ESP8266WebServer server(80);
RelayManager relay(relayPin);
TimeManager timeManager;


void setup() {
    Serial.begin(115200);

    // Setup WiFi
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        Serial.println("Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }

    // Setup mDNS
    if (!MDNS.begin("bellsystem")) { 
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
        MDNS.begin("bellsystem"); // Start the mDNS responder with URL bellSystem.local
    }

    // Setup routes
    server.on("/", HTTP_GET, []() {
      Serial.println("Home page called");
      WebPage homePage("Home");
      String homePageHtml = homePage.generatePage();
      server.send(200, "text/html", homePageHtml);
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




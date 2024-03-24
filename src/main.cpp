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
#include <LittleFS.h>
#include <base64.h>
#include <ArduinoJson.h>
#include <ezTime.h>

#include "board/EEPROMLayoutManager.h"
#include "schedule/TimeManager.h"
#include "schedule/scheduleManager.h"
#include "board/RelayManager.h"
#include "web/Endpoints.h"
#include "web/AuthManager.h"

// Pins used for reset trigger and ground
#define RESET_TRIGGER_PIN 14 // Reset trigger pin (D5, GPIO 14)
#define GROUND_PIN D6 // Ground pin
#define DEBOUNCE_DELAY 500 // 500 milliseconds debounce delay

// Global objects
EEPROMLayoutManager eepromManager; // EEPROM manager object
const int relayPin = 5; // Pin for the relay module (D1, GPIO 5) 
ESP8266WebServer server(80); // HTTP server object
RelayManager relayManager(relayPin); // Relay manager object
TimeManager timeManager; // Time manager object
ScheduleManager scheduleManager; // Schedule manager object
AuthManager authManager; // Authentication manager object

String deviceName; // Device name
String uniqueURL; // Unique URL for the device
int ringDuration; // Ring duration in seconds
unsigned long lastRingTime = 0; // Last time the bell rang
const long checkInterval = 6000; // Interval to check if the bell should ring (1 minute)
DynamicJsonDocument systemMessages(1024); // Array to store system messages



void setup() {
    pinMode(RESET_TRIGGER_PIN, INPUT_PULLUP); // Set the reset trigger pin as an input

    // Initialize the system messages array
    systemMessages.to<JsonArray>();

    // Add a small delay to allow for any conditions to stabilize
    delay(DEBOUNCE_DELAY);

    // Check if the reset condition is met
    // To reset the password to default, and clear WiFi credentials short GPIO 14 to ground WHILE pressing the reset button and hold 
    // for 1 second before releasing both.
    if (digitalRead(RESET_TRIGGER_PIN) == LOW) {
        eepromManager.saveInitialized(false);   // Reset the password to default on reboot
        eepromManager.saveDeviceName("bellsystem"); // Reset the device name to default
        eepromManager.saveUniqueURL("bellsystem"); // Reset the unique URL to default
        eepromManager.saveRingDuration(2); // Reset the ring duration to default

        // Now perform the restart
        ESP.restart();
    }

    /********************************************************************************
     * Setup services and objects, and load settings from EEPROM
    ********************************************************************************/   

    // Initialize EEPROM with 4096 bytes and check if it was successful
    if(!eepromManager.begin(4096)) {
        eepromManager.addSystemMessage("EEPROM initialization failed.");
    } else {
        eepromManager.addSystemMessage("EEPROM initialized successfully");
    }


    // Initialize schedule manager, authentication manager, and relay manager objects
    scheduleManager = ScheduleManager();
    authManager.initialize();
    relayManager = RelayManager(relayPin);

    // Load settings from EEPROM
    deviceName = eepromManager.loadDeviceName();
    uniqueURL = eepromManager.loadUniqueURL();
    ringDuration = eepromManager.loadRingDuration();


    // Initialize LittleFS file system and check if it was successful
    if (!LittleFS.begin()) {
        eepromManager.addSystemMessage("LittleFS initialization failed.");
        return;
    } else {
        eepromManager.addSystemMessage("LittleFS mounted successfully");
    }

    // Setup WiFi manager and connect to WiFi network if not already connected
    WiFiManager wifiManager;

    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        ESP.restart();
        delay(1000);
    }

    // Setup mDNS responder and check if it was successful
    // Set the unique URL to the device name set by the user (default is bellsystem)
    if (!MDNS.begin(uniqueURL)) { 
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "MDNS responder failed to start. IP: %s", WiFi.localIP().toString().c_str());
        systemMessages.add(buffer);
    } else {
        char buffer[100]; // Adjust size as needed
        snprintf(buffer, sizeof(buffer), "MDNS started with URL: %s.local", uniqueURL.c_str());
        systemMessages.add(buffer);
        MDNS.addService("http", "tcp", 80);
    }

    // Setup the time manager and begin the NTP client
    timeManager.begin();

    // Setup endpoints for the HTTP server
    setupEndpoints();

    server.begin(); // Start the HTTP server
}

void loop() {
    events();
    MDNS.update();

    // Check if the bell should ring, every minute
    if (minuteChanged()) {
        scheduleManager.handleRing();
    }


    server.handleClient();
}




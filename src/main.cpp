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



void setup() {
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as an output
    pinMode(RESET_TRIGGER_PIN, INPUT_PULLUP); // Set the reset trigger pin as an input

    Serial.begin(115200); // Start serial communication at 115200 baud

    // Add a small delay to allow for any transient conditions to stabilize
    delay(DEBOUNCE_DELAY);

    // Check if the reset condition is met
    // To reset the password to default, and clear WiFi credentials short GPIO 14 to ground WHILE pressing the reset button and hold for 1 second before releasing both.
    if (digitalRead(RESET_TRIGGER_PIN) == LOW) { // Assuming active low; use HIGH for active high
        Serial.println("Reset condition detected. Clearing settings...");

        eepromManager.saveInitialized(false);   // Reset the password to default on reboot
        eepromManager.saveResetWifi(true);       // Clear WiFi credentials on reboot

        // Now perform the restart
        ESP.restart();
    }

    /********************************************************************************
     * Setup services and objects, and load settings from EEPROM
    ********************************************************************************/   

    // Initialize EEPROM with 4096 bytes and check if it was successful
    if(!eepromManager.begin(4096)) {
        Serial.println("Failed to initialize EEPROM");
    }


    // Initialize schedule manager, authentication manager, and relay manager objects
    scheduleManager = ScheduleManager();
    authManager.initialize();
    relayManager = RelayManager(relayPin);

    // See if we need to reset the WiFi credentials
    bool resetWifi = eepromManager.loadResetWifi();

    // Load settings from EEPROM
    deviceName = eepromManager.loadDeviceName();
    uniqueURL = eepromManager.loadUniqueURL();
    ringDuration = eepromManager.loadRingDuration();
    Serial.println("Device name: " + deviceName);
    Serial.println("Unique URL: " + uniqueURL);
    Serial.println("Ring duration: " + String(ringDuration));


    // Initialize LittleFS file system and check if it was successful
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    // Setup WiFi manager and connect to WiFi network if not already connected
    // If resetWifi is true, start access point to connect to a new network
    WiFiManager wifiManager;

    if (resetWifi) {
        wifiManager.setConfigPortalTimeout(180);
        if (!wifiManager.startConfigPortal("BellSystemSetupAP")) {
            delay(1000);
            ESP.restart();
            delay(1000);
        }
        eepromManager.saveResetWifi(false);
    } else {
        if (!wifiManager.autoConnect("BellSystemSetupAP")) {
            ESP.restart();
            delay(1000);
        }
    }

    // Setup mDNS responder and check if it was successful
    // Set the unique URL to the device name set by the user (default is bellsystem)
    if (!MDNS.begin(uniqueURL)) { 
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started:" + uniqueURL);
        MDNS.addService("http", "tcp", 80);
    }

    // Setup the time manager and begin the NTP client
    timeManager.begin();

    // Setup endpoints for the HTTP server
    setupEndpoints();

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    events();

    // Check if the bell should ring, every minute
    if (minuteChanged()) {
        scheduleManager.handleRing();
    }


    server.handleClient();
    MDNS.update();
}




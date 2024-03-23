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
#define RESET_TRIGGER_PIN D5 // Reset trigger pin
#define GROUND_PIN D6 // Ground pin

// Global objects
EEPROMLayoutManager eepromManager; // EEPROM manager object
const int relayPin = 5; // Pin for the relay module (GPIO5) 
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
bool isButtonBeingPressed = false; // Flag to check if the button is being pressed
unsigned long buttonPressedTime = 0; // Time the button was pressed


void setup() {
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as an output
    pinMode(RESET_TRIGGER_PIN, INPUT_PULLUP);
    pinMode(GROUND_PIN, OUTPUT);
    digitalWrite(GROUND_PIN, HIGH); // Keep it high normally

    Serial.begin(115200); // Start serial communication at 115200 baud

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
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        ESP.restart();
        delay(1000);
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
    // Logic to reset the wifi credentials and password
    if (digitalRead(RESET_TRIGGER_PIN) == LOW) { // Button is pressed
        if (!isButtonBeingPressed) { // If it's the start of the press
            isButtonBeingPressed = true;
            digitalWrite(LED_BUILTIN, LOW); // Turn off the LED
            buttonPressedTime = millis();
        } else if (millis() - buttonPressedTime > 5000) { // Button held for 5 seconds
            Serial.println("Resetting WiFi credentials and reverting to defaults...");
            eepromManager.saveInitialized(false);   // Reset the password to default on reboot
            WiFiManager wifiManager;
            wifiManager.resetSettings(); // Clear WiFi credentials
            ESP.restart(); // Restart the ESP
        }
    } else {
        isButtonBeingPressed = false; // Button not being pressed
        buttonPressedTime = 0; // Reset button pressed time
    }


    events();

    // Check if the bell should ring, every minute
    if (minuteChanged()) {
        scheduleManager.handleRing();
    }


    server.handleClient();
    MDNS.update();
}




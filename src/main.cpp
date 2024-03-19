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

#include "EEPROMLayoutManager.h"
#include "TimeManager.h"
#include "ScheduleManager.h"
#include "RelayManager.h"

// Global objects
EEPROMLayoutManager eepromManager; // EEPROM manager object
const int relayPin = 5; // Pin for the relay module (GPIO5) 
ESP8266WebServer server(80); // HTTP server object
RelayManager relay(relayPin); // Relay manager object
TimeManager timeManager; // Time manager object
ScheduleManager scheduleManager; // Schedule manager object


void setup() {
    Serial.begin(115200); // Start serial communication at 115200 baud

    /********************************************************************************
     * Setup services and objects
    ********************************************************************************/   

    // Initialize EEPROM with 2048 bytes and check if it was successful
    if(!eepromManager.begin(2048)) {
        Serial.println("Failed to initialize EEPROM");
    }

    // Initialize LittleFS file system and check if it was successful
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    // Attempt to load settings from EEPROM
    String deviceName = eepromManager.loadDeviceName();
    String uniqueURL = eepromManager.loadUniqueURL();
    Serial.println("Device name: " + deviceName); // Debugging
    Serial.println("Unique URL: " + uniqueURL); // Debugging

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

    /************************************************************************************
     * Setup the HTTP server and define the endpoints
     * Files were too big to be sent in one go, so they are sent in chunks.
    ************************************************************************************/

    /*************************Schedule Page*************************************/

    server.on("/getSchedule", HTTP_GET, []() {
        String providedToken = server.header("Authorization");

        if (!eepromManager.checkSessionToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        // Load the schedule string from EEPROM
        String scheduleString = eepromManager.loadRingSchedule();
        Serial.println("Schedule loaded: " + scheduleString);
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, scheduleString);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }

        server.send(200, "application/json", scheduleString);
    });


    server.on("/updateSchedule", HTTP_POST, []() {
        String providedToken = server.header("Authorization");

        if (!eepromManager.checkSessionToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        Serial.println("SaveSchedule called");

        // Check if the request has a body (raw POST data)
        if (server.hasArg("plain") == false) { // Use "plain" for raw POST body
            Serial.println("No schedule data received");
            server.send(400, "text/plain", "No schedule data received");
            return;
        }

        // Getting the raw POST data
        String schedule = server.arg("plain");
        DynamicJsonDocument doc(1024); // Adjust size according to your needs
        DeserializationError error = deserializeJson(doc, schedule);
        
        if (error) { // Check for errors in parsing
            Serial.println("parseObject() failed");
            server.send(500, "text/plain", "Error parsing JSON");
            return;
        }

        // Now you can use the JSON object, for example:
        // Serialize JSON to String and save it
        if (eepromManager.saveRingSchedule(schedule)) {
            server.send(200, "text/plain", "Schedule saved successfully");
        } else {
            server.send(500, "text/plain", "Failed to save schedule");
        }
    });

    server.on("/script/schedule.js", HTTP_GET, []() {
        File file = LittleFS.open("/script/schedule.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    server.on("/schedule", HTTP_GET, []() {
        File file = LittleFS.open("/schedule.html", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        String htmlContent;
        htmlContent = file.readString();
        file.close();

        // Replace the placeholder with the device name
        htmlContent.replace("{{deviceName}}", eepromManager.loadDeviceName());

        server.send(200, "text/html", htmlContent);
    });

    /*************************Login Page*************************************/

    server.on("/login", HTTP_GET, []() {
        File file = LittleFS.open("/login.html", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        server.streamFile(file, "text/html");
        file.close();
    });

    server.on("/completeLogin", HTTP_POST, []() {
        if (server.hasArg("password")) {
            Serial.println("Password received");
            String password = server.arg("password");
            if (password == eepromManager.loadPassword()) {
                Serial.println("Password correct");
                String token = eepromManager.generateRandomToken();
                eepromManager.saveSessionToken(token);
                String jsonResponse = "{\"token\":\"" + token + "\"}";
                server.send(200, "application/json", jsonResponse);
            } else {
                server.send(401, "text/plain", "Unauthorized");
            }
        } else {
            server.send(400, "text/plain", "No password received");
        }
    });

    server.on("/script/login.js", HTTP_GET, []() {
        String jsContent;
        File file = LittleFS.open("/script/login.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    /*************************Authentication*************************************/

    server.on("/script/auth.js", HTTP_GET, []() {
        String jsContent;
        File file = LittleFS.open("/script/auth.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    /*************************Home Page*************************************/

    server.on("/script/index.js", HTTP_GET, []() {
        String jsContent;
        File file = LittleFS.open("/script/index.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    server.on("/ToggleRelay", HTTP_GET, []() {
        String providedToken = server.header("Authorization");

        if (!eepromManager.checkSessionToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        Serial.println("ToggleRelay called");
        int duration = eepromManager.loadRingDuration();
        relay.activateRelay(duration); // Activate the relay for saved duration
        server.send(200, "text/plain", "Relay toggle successful");
    });

    server.on("/", HTTP_GET, []() {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        String htmlContent;
        htmlContent = file.readString();
        file.close();

        // Replace the placeholder with the device name
        htmlContent.replace("{{deviceName}}", eepromManager.loadDeviceName());
        htmlContent.replace("{{dateTime}}", timeManager.getDateTime());

        server.send(200, "text/html", htmlContent);
    });

    /*************************Settings Page*************************************/

    server.on("/settings", HTTP_GET, []() {
        String htmlContent;
        File file = LittleFS.open("/settings.html", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        htmlContent = file.readString();
        file.close();

        // Replace the placeholder with the device name
        htmlContent.replace("{{deviceName}}", eepromManager.loadDeviceName());
        htmlContent.replace("{{uniqueURL}}", eepromManager.loadUniqueURL());
        htmlContent.replace("{{ringDuration}}", String(eepromManager.loadRingDuration()));

        server.send(200, "text/html", htmlContent);
    });

    server.on("/saveSettings", HTTP_POST, []() {
        // Assuming the token is sent in the Authorization header
        String providedToken = server.header("Authorization");

        if (!eepromManager.checkSessionToken(providedToken)) {
            // If the token check fails, respond with 401 Unauthorized
            server.send(401, "text/plain", "Unauthorized");
            return; // Stop further processing
        }

        Serial.println("SaveSettings called");
        if (server.hasArg("deviceName")) {
            String deviceName = server.arg("deviceName");
            eepromManager.saveDeviceName(deviceName);
            Serial.print("Device name received: ");
            Serial.println(deviceName);
        }
        if (server.hasArg("uniqueURL")) {
            String uniqueURL = server.arg("uniqueURL");
            eepromManager.saveUniqueURL(uniqueURL);
            Serial.print("Unique URL received: ");
            Serial.println(uniqueURL);
        }
        if (server.hasArg("ringDuration")) {
            int ringDuration = server.arg("ringDuration").toInt();
            eepromManager.saveRingDuration(ringDuration);
            Serial.print("Ring duration received: ");
            Serial.println(ringDuration);
        }

        server.send(200, "text/plain", "Settings saved");
    });

        server.on("/script/settings.js", HTTP_GET, []() {
        String jsContent;
        File file = LittleFS.open("/script/settings.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    /*************************Change Password Page*************************************/

    server.on("/changepassword", HTTP_GET, []() {
        File file = LittleFS.open("/changePassword.html", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        String htmlContent;
        htmlContent = file.readString();
        file.close();

        // Replace the placeholder with the device name
        htmlContent.replace("{{deviceName}}", eepromManager.loadDeviceName());

        server.send(200, "text/html", htmlContent);
    });

    server.on("/script/changePassword.js", HTTP_GET, []() {
        String jsContent;
        File file = LittleFS.open("/script/changePassword.js", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }
        

        //No changes need to be made to the JS file, so send it as is
        server.streamFile(file, "text/javascript");
        server.sendHeader("Cache-Control", "max-age=86400"); // Cache for 1 day to reduce server load
        file.close();
    });

    server.on("/finalizePassword", HTTP_POST, []() {
        Serial.println("ChangePassword called");

        String providedToken = server.header("Authorization");
        if (!eepromManager.checkSessionToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        if (server.hasArg("OldPassword") && server.hasArg("NewPassword")) {
            String oldPassword = server.arg("OldPassword");
            String newPassword = server.arg("NewPassword");
            String storedPassword = eepromManager.loadPassword();
            oldPassword.trim();
            newPassword.trim();

            if (oldPassword == storedPassword) { 
                eepromManager.savePassword(newPassword);
                server.send(200, "text/plain", "Password changed successfully.");
            } else {
                server.send(401, "text/plain", "Invalid old password.");
            }
        } else {
            server.send(400, "text/plain", "Required parameters missing.");
        }
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




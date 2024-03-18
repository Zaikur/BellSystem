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
#include "RelayManager.h"

// Global objects
EEPROMLayoutManager eepromManager; // EEPROM manager object
const int relayPin = 5; // Pin for the relay module (GPIO5) 
ESP8266WebServer server(80); // HTTP server object
RelayManager relay(relayPin); // Relay manager object
TimeManager timeManager; // Time manager object

void setup() {
    Serial.begin(115200); // Start serial communication at 115200 baud

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

    // Setup WiFi manager and connect to WiFi network if not already connected
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("BellSystemSetupAP")) {
        Serial.println("Failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }

    // Setup mDNS responder and check if it was successful
    // Set the unique URL to the device name set by the user (default is bellsystem)
    if (!MDNS.begin(uniqueURL)) { 
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
    }

    // Setup route endpoints for the HTTP server
    // Files were too big to be sent in one go, so they are sent in chunks.
    server.on("/getSchedule", HTTP_GET, []() {          //ADD Server side validation                                    ADD SCHEDULE SAVING AND RETRIEVING
        StaticJsonDocument<1024> doc; // For a known, manageable JSON size

        // Correctly obtaining nested arrays according to new recommendations
        JsonArray monday = doc["monday"].to<JsonArray>();
        monday.add("08:00");
        monday.add("12:00");

        JsonArray tuesday = doc["tuesday"].to<JsonArray>();
        tuesday.add("09:00");
        tuesday.add("13:00");

        String scheduleString;
        ArduinoJson::serializeJson(doc, scheduleString);

        server.send(200, "application/json", scheduleString); // Send schedule as JSON string
    });

    server.on("/saveSchedule", HTTP_POST, []() {
        String providedToken = server.header("Authorization");

        if (!eepromManager.checkSessionToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        Serial.println("SaveSchedule called");
        if (server.hasArg("schedule")) {
            String schedule = server.arg("schedule");
            eepromManager.saveRingSchedule(schedule); // Ensure schedule format and storage method is secure and efficient
            Serial.print("Schedule received: ");
            Serial.println(schedule);
            server.send(200, "text/plain", "Schedule saved successfully");
        } else {
            server.send(400, "text/plain", "No schedule data received");
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

    //This method will be called when / is requested, it will send a response to the client with the content of the HTML file
    //after replacing the placeholder with the device name
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

        server.send(200, "text/html", htmlContent);
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




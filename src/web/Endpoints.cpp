/*
Quinton Nelson
3/22/2024
This file contains the endpoints for the web server
Sending all necessary files and data to the client at once is not feasible, so the files are sent in chunks.
*/


#include "Endpoints.h"

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ezTime.h>

#include "board/EEPROMLayoutManager.h"
#include "schedule/TimeManager.h"
#include "schedule/scheduleManager.h"
#include "board/RelayManager.h"
#include "web/AuthManager.h"

// Global objects that are defined in main.cpp
extern EEPROMLayoutManager eepromManager; // EEPROM manager object
extern const int relayPin; // Pin for the relay module (GPIO5)
extern ESP8266WebServer server; // HTTP server object
extern RelayManager relayManager; // Relay manager object
extern TimeManager timeManager; // Time manager object
extern ScheduleManager scheduleManager; // Schedule manager object
extern AuthManager authManager; // Authentication manager object


extern String deviceName; // Device name
extern String uniqueURL; // Unique URL for the device
extern int ringDuration; // Ring duration in seconds



void setupEndpoints() {
    /*************************Schedule Page*************************************/

    server.on("/getSchedule", HTTP_GET, []() {
        // Ensure no caching for dynamic content
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");

        String scheduleJson = scheduleManager.getScheduleString(); // Get the schedule as a string
        server.send(200, "application/json", scheduleJson);
    });



    server.on("/updateSchedule", HTTP_POST, []() {
        String providedToken = server.header("Authorization");

        if (!authManager.checkToken(providedToken)) {
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

        // Serialize JSON to String and save it
        if (scheduleManager.updateSchedule(schedule)) {
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
        htmlContent.replace("{{deviceName}}", deviceName);

        server.send(200, "text/html", htmlContent);
    });

    /*************************Authentication*************************************/

    server.on("/completeLogin", HTTP_POST, []() {
        if (server.hasArg("password")) {
            Serial.println("Password received");
            String password = server.arg("password");
            if (authManager.checkPassword(password)) {
                Serial.println("Password correct");
                String token = authManager.generateToken();
                String jsonResponse = "{\"token\":\"" + token + "\"}";
                server.send(200, "application/json", jsonResponse);
            } else {
                server.send(401, "text/plain", "Unauthorized");
            }
        } else {
            server.send(400, "text/plain", "No password received");
        }
    });

    server.on("/auth", HTTP_GET, []() {
        String providedToken = server.header("Authorization");

        if (!authManager.checkToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        server.send(200, "text/plain", "Authorized");
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
        file.close();
    });

    server.on("/ToggleRelay", HTTP_GET, []() {
        String providedToken = server.header("Authorization");

        if (!authManager.checkToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        Serial.println("ToggleRelay called");
        relayManager.activateRelay(); // Activate the relay for saved duration
        server.send(200, "text/plain", "Relay toggle successful");
    });

    server.on("/getTodayRemainingRingTimes", HTTP_GET, []() {
        String result = scheduleManager.getTodayRemainingRingTimes(); // Get the remaining ring times for today
        Serial.println("Remaining ring times: " + result);
        server.send(200, "text/plain", result);
    });

    server.on("/getServerMessages", HTTP_GET, []() {
        // Get the server messages from EEPROM********************************************************
        String serverMessages = "";
        server.send(200, "text/plain", serverMessages);
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
        htmlContent.replace("{{deviceName}}", deviceName);
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
        htmlContent.replace("{{deviceName}}", deviceName);
        htmlContent.replace("{{uniqueURL}}", uniqueURL);
        htmlContent.replace("{{ringDuration}}", String(ringDuration));

        // Set Cache-Control headers
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");

        server.send(200, "text/html", htmlContent);
    });

#include <ArduinoJson.h> // Make sure this include is at the top of your file

server.on("/saveSettings", HTTP_POST, []() {
    // Assuming the token is sent in the Authorization header
    String providedToken = server.header("Authorization");

    if (!authManager.checkToken(providedToken)) {
        // If the token check fails, respond with 401 Unauthorized
        server.send(401, "text/plain", "Unauthorized");
        return; // Stop further processing
    }

    // Read the raw POST data
    String requestBody = server.arg("plain");
    
    // Use ArduinoJson to parse the JSON payload
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, requestBody);
    
    if (error) {
        server.send(500, "text/plain", "Error parsing JSON!");
        return;
    }

    Serial.println("SaveSettings called");

    // Extract and save the device name
    if (doc.containsKey("deviceName")) {
        deviceName = doc["deviceName"].as<String>();
        eepromManager.saveDeviceName(deviceName);
        Serial.print("Device name received: ");
        Serial.println(deviceName);
    }

    // Extract and save the ring duration
    if (doc.containsKey("ringDuration")) {
        ringDuration = doc["ringDuration"];
        eepromManager.saveRingDuration(ringDuration);
        Serial.print("Ring duration received: ");
        Serial.println(ringDuration);
    }

    // Extract, compare, and potentially save the unique URL
    if (doc.containsKey("uniqueURL")) {
        uniqueURL = doc["uniqueURL"].as<String>();
        if (uniqueURL != eepromManager.loadUniqueURL()) {
            eepromManager.saveUniqueURL(uniqueURL);
            Serial.println("Unique URL updated: " + uniqueURL);
            server.send(200, "text/plain", "URL saved successfully, device will restart to apply changes");
            delay(1000); // Short delay before restart
            ESP.restart();
            return;
        }
    }

    server.send(200, "text/plain", "Settings saved successfully.");
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
        htmlContent.replace("{{deviceName}}", deviceName);

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
        file.close();
    });

    server.on("/finalizePassword", HTTP_POST, []() {
        Serial.println("ChangePassword called");

        String providedToken = server.header("Authorization");
        if (!authManager.checkToken(providedToken)) {
            server.send(401, "text/plain", "Unauthorized");
            return;
        }

        if (server.hasArg("OldPassword") && server.hasArg("NewPassword")) {
            if (authManager.checkPassword(server.arg("OldPassword"))) {
                if (authManager.updatePassword(server.arg("NewPassword"))) {
                    server.send(200, "text/plain", "Password changed successfully.");
                } else {
                    server.send(500, "text/plain", "Failed to save new password.");
                }
            } else {
                server.send(401, "text/plain", "Invalid old password.");
            }
        } else {
            server.send(400, "text/plain", "Required parameters missing.");
        }
    });

    /*************************Favicon*************************************/

    server.on("/favicon.ico", HTTP_GET, []() {
        File file = LittleFS.open("/favicon.ico", "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        server.streamFile(file, "image/x-icon");
    });

    // Handle not found
    server.onNotFound([]() {
        server.send(404, "text/plain", "Not found");
    });
}
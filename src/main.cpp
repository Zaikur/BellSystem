/*
Quinton Nelson
3/13/2024
This file handles the main loop and setup of the program. It initializes the global objects and handles the main loop of the program.
*/

#include <Arduino.h>
#include <EEPROM.h> // Include the EEPROM library for reading and writing to non-volatile memory

#include <base64.h> // Include the base64 library for encoding and decoding

#include "WebPage.h"
#include "Server.h"
#include "RelayManager.h"
#include "TimeManager.h"
#include "MyWiFiManager.h"

#define RELAY_PIN 5 // Define the pin for the relay

// Global objects
Server server;    // Create an instance of the Server class
MyWiFiManager myWiFiManager;
RelayManager relayManager(RELAY_PIN);
TimeManager timeManager;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512); // Initialize EEPROM with desired size

  myWiFiManager.setupWiFi(); // Connect to WiFi using the MyWiFiManager class
  timeManager.updateTime(); // Update the time using the TimeManager class
}

void loop() {
  server.handleClientRequests();
}




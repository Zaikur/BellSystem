/*
Quinton Nelson
3/22/2024
This file contains the endpoints for the web server
*/

#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

extern DynamicJsonDocument systemMessages; // Array to store system messages

//Initialization function
void setupEndpoints();

#endif
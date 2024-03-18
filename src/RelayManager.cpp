/*
Quinton Nelson
3/13/2024
This file handles relay activation
*/

#include "RelayManager.h"

// Constructor for the RelayManager class
RelayManager::RelayManager(int pin) : relayPin(pin) {
    pinMode(relayPin, OUTPUT);
}

// This method activates the relay for a given duration
void RelayManager::activateRelay(int duration) {
    Serial.println("Activating relay");
    digitalWrite(relayPin, HIGH);
    delay(duration * 1000);
    digitalWrite(relayPin, LOW);
}

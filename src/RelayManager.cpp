/*
Quinton Nelson
3/13/2024
This file handles relay activation
*/

#include "RelayManager.h"

RelayManager::RelayManager(int pin) : relayPin(pin) {
    pinMode(relayPin, OUTPUT);
}

void RelayManager::activateRelay(int duration) {
    Serial.println("Activating relay");
    digitalWrite(relayPin, HIGH);
    delay(duration * 1000);
    digitalWrite(relayPin, LOW);
}

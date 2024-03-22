/*
Quinton Nelson
3/13/2024
This file handles relay activation
*/

#ifndef RelayManager_h
#define RelayManager_h

#include <Arduino.h>

// Global variable initialized in main.cpp
extern int ringDuration;

class RelayManager {
public:
    RelayManager(int pin);
    void activateRelay();
private:
    int relayPin;
};

#endif

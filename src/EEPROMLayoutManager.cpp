/*
Quinton Nelson
3/15/2024
This file handles EEPROM management
Contains methods to save and load data from EEPROM including ring schedule, ring duration, and device name
*/


#include "EEPROMLayoutManager.h"

EEPROMLayoutManager::EEPROMLayoutManager() {}

void EEPROMLayoutManager::begin(size_t size) {
    EEPROM.begin(size);
}

bool EEPROMLayoutManager::saveRingSchedule(const String& schedule) {
    return saveString(schedule, scheduleStartAddr);
}

bool EEPROMLayoutManager::wipeSavedSchedule() {
    return saveString("", scheduleStartAddr);
}

bool EEPROMLayoutManager::saveRingDuration(int duration) {
    return saveInt(duration, ringDurationStartAddr);
}

int EEPROMLayoutManager::loadRingDuration() {
    return loadInt(ringDurationStartAddr);
}

bool EEPROMLayoutManager::saveDeviceName(const String& deviceName) {
    return saveString(deviceName, deviceNameAddr);
}

String EEPROMLayoutManager::loadDeviceName() {
    String name = loadString(deviceNameAddr, 100); // Assuming max 100 characters for device name
    if(name.length() > 0) {
        name += "bellsystem"; // Append "bellsystem" to the device name
        Serial.print("Device name loaded and modified: ");
    } else {
        name = "bellsystem"; // Use default name
    }
}

bool EEPROMLayoutManager::saveString(const String& data, int startAddr) {
    int i;
    for (i = 0; i < data.length(); i++) {
        EEPROM.write(startAddr + i, data[i]);
    }
    EEPROM.write(startAddr + i, '\0'); // Null-terminate the string
    return EEPROM.commit();
}

String EEPROMLayoutManager::loadString(int startAddr, int maxLen) {
    String result = "";
    for (int i = 0; i < maxLen; i++) {
        char ch = EEPROM.read(startAddr + i);
        if (ch == '\0') break;
        result += ch;
    }
    return result;
}

bool EEPROMLayoutManager::saveInt(int value, int startAddr) {
    EEPROM.put(startAddr, value);
    return EEPROM.commit();
}

int EEPROMLayoutManager::loadInt(int startAddr) {
    int value;
    EEPROM.get(startAddr, value);
    return value;
}

size_t EEPROMLayoutManager::findEEPROMEnd() {
    // Implement logic to dynamically find the end of the stored data, if needed
}

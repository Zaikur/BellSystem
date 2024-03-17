/*
Quinton Nelson
3/15/2024
This file handles EEPROM management
Contains methods to save and load data from EEPROM including ring schedule, ring duration, and device name
*/


#include "EEPROMLayoutManager.h"

EEPROMLayoutManager::EEPROMLayoutManager() {}

bool EEPROMLayoutManager::begin(size_t size) {
    EEPROM.begin(size);
    return true;
}

bool EEPROMLayoutManager::saveRingSchedule(const String& schedule) {
    return saveString(schedule, scheduleStartAddr);
}

bool EEPROMLayoutManager::wipeSavedSchedule() {
    return saveString("", scheduleStartAddr);
}

String EEPROMLayoutManager::loadRingSchedule() {
    return loadString(scheduleStartAddr, 1000);
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
    String deviceName = loadString(deviceNameAddr, 100);
    if (deviceName.length() > 0 && deviceName[0] == char(0xFF)) {
        return "bellsystem"; // Return the default device name
    } else {
        return deviceName; // Return the loaded device name
    }
}

bool EEPROMLayoutManager::savePassword(const String& password) {
    return saveString(password, passwordAddr);
}

String EEPROMLayoutManager::loadPassword() {
    String password =  loadString(passwordAddr, 100);
    if (password.length() > 0 && password[0] == char(0xFF)) {
        return "password"; // Return the default password
    } else {
        return password; // Return the loaded password
    }
}

bool EEPROMLayoutManager::saveString(const String& data, int startAddr) {
    unsigned int i;
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

// Token generation and storage
String EEPROMLayoutManager::generateRandomToken() {
    String token = "";
    for (int i = 0; i < 16; i++) {
        token += (char)random(33, 126);
    }
    return token;
}

bool EEPROMLayoutManager::saveSessionToken(const String& token) {
    return saveString(token, 800);
}

bool EEPROMLayoutManager::checkSessionToken(const String& token) {
    return token == loadString(800, 16);
}

bool EEPROMLayoutManager::wipeSessionToken() {
    return saveString("", 800);
}
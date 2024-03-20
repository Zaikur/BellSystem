/*
Quinton Nelson
3/15/2024
This file handles EEPROM management
Contains methods to save and load data from EEPROM including ring schedule, ring duration, and device name
Start address for each data type is defined as a constant, and methods are provided to save and load data from these addresses
*/


#include "EEPROMLayoutManager.h"

EEPROMLayoutManager::EEPROMLayoutManager() {}

bool EEPROMLayoutManager::begin(size_t size) {
    EEPROM.begin(size);
    return true;
}



// Ring schedule management
bool EEPROMLayoutManager::saveRingSchedule(const String& schedule) {
    bool saveSuccess = saveString(schedule, scheduleStartAddr);
    // Immediately read back for verification
    return saveSuccess;
}


bool EEPROMLayoutManager::wipeSavedSchedule() {
    return saveString("", scheduleStartAddr);
}

String EEPROMLayoutManager::loadRingSchedule() {
    String schedule = loadString(scheduleStartAddr, 1000);
    if (schedule.length() > 0 && schedule[0] == char(0xFF)) {
        return ""; // Return an empty string if the schedule is empty
    } else {
        return schedule; // Return the loaded schedule
    }
}

// Ring duration management
bool EEPROMLayoutManager::saveRingDuration(int duration) {
    return saveInt(duration, ringDurationStartAddr);
}

int EEPROMLayoutManager::loadRingDuration() {
    return loadInt(ringDurationStartAddr);
}

// Device name management
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

// Unique URL management
String EEPROMLayoutManager::loadUniqueURL() {
    String url = loadString(uniqueURLAddr, 100);
    if (url.length() > 0 && url[0] == char(0xFF)) {
        return "bellsystem"; // Return the default device name
    } else {
        return url; // Return the loaded device name
    }
}

bool EEPROMLayoutManager::saveUniqueURL(const String& url) {
    return saveString(url, uniqueURLAddr);
}

// Password management
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

// Token generation and storage
String EEPROMLayoutManager::generateRandomToken() {
    String token = "";
    for (int i = 0; i < 16; i++) {
        token += (char)random(33, 126);
    }
    return token;
}

bool EEPROMLayoutManager::saveSessionToken(const String& token) {
    return saveString(token, sessionTokenAddr);
}

bool EEPROMLayoutManager::checkSessionToken(const String& token) {
    return token == loadString(sessionTokenAddr, 16);
}

bool EEPROMLayoutManager::wipeSessionToken() {
    return saveString("", 800);
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

    Serial.println("Loaded string: " + result);
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
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



/****************************Ring schedule****************************/
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

/****************************Ring duration****************************/
bool EEPROMLayoutManager::saveRingDuration(int duration) {
    return saveInt(duration, ringDurationStartAddr);
}

int EEPROMLayoutManager::loadRingDuration() {
    int duration = loadInt(ringDurationStartAddr);
    Serial.println("Loaded ring duration: " + String(duration));
    if (duration == 0) {
        return 2; // Return the default ring duration
    } else if (duration > 10 || duration < 0) {
        return 2;
    } else {
        return duration; // Return the loaded ring duration
    }
}

/****************************Device settings****************************/
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

/*****************************Unique URL******************************/
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

/*****************************Password******************************/
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

bool EEPROMLayoutManager::saveSalt(const String& salt) {
    return saveString(salt, saltAddr);
}

String EEPROMLayoutManager::loadSalt() {
    String salt = loadString(saltAddr, 100);
    return salt;
}

bool EEPROMLayoutManager::saveInitialized(bool initialized) {
    return saveInt(initialized, initializedAddr);
}

bool EEPROMLayoutManager::loadInitialized() {
    int initialized = loadInt(initializedAddr);
    if (initialized <= 0 || initialized > 1) {
        return false; // Return false if the device is not initialized
    } else {
        return true; // Return true if the device is initialized
    }
}

/****************************Private methods****************************/
bool EEPROMLayoutManager::saveString(const String& data, int startAddr) {
    unsigned int i;
    Serial.println("Saving string: " + data);
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
    Serial.println("Loaded int: " + String(value));
    return value;
}
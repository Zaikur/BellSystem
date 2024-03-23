/*
Quinton Nelson
3/15/2024
This file handles EEPROM management
*/

#ifndef EEPROMLayoutManager_h
#define EEPROMLayoutManager_h

#include <Arduino.h>
#include <EEPROM.h>

class EEPROMLayoutManager {
public:
    EEPROMLayoutManager();
    bool begin(size_t size);
    bool saveRingSchedule(const String& schedule);
    bool wipeSavedSchedule();
    String loadRingSchedule();

    bool saveRingDuration(int duration);
    int loadRingDuration();

    bool saveDeviceName(const String& deviceName);
    String loadDeviceName();

    bool savePassword(const String& password);
    String loadPassword();
    bool saveSalt(const String& salt);
    String loadSalt();
    bool saveInitialized(bool initialized);
    bool loadInitialized();

    bool saveUniqueURL(const String& url);
    String loadUniqueURL();

private:
    size_t findEEPROMEnd();
    bool saveString(const String& data, int startAddr);
    String loadString(int startAddr, int maxLen);
    bool saveInt(int value, int startAddr);
    int loadInt(int startAddr);
    
    const int ringDurationStartAddr = 100;
    const int deviceNameAddr = 200;
    const int uniqueURLAddr = 300;
    const int passwordAddr = 400;
    const int saltAddr = 500;
    const bool initializedAddr = 600;
    // DoubleResetDetector start address is 700
    const int scheduleStartAddr = 2000;
};

#endif
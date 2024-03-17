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
    bool wipeSavedRingDuration();
    int loadRingDuration();
    bool saveDeviceName(const String& deviceName);
    bool savePassword(const String& password);
    String loadPassword();
    String loadDeviceName();
    bool saveUniqueURL(const String& url);
    String loadUniqueURL();
    // Token generation and storage
    String generateRandomToken();
    bool saveSessionToken(const String& token);
    bool checkSessionToken(const String& token);
    bool wipeSessionToken();


private:
    size_t findEEPROMEnd();
    bool saveString(const String& data, int startAddr);
    String loadString(int startAddr, int maxLen);
    bool saveInt(int value, int startAddr);
    int loadInt(int startAddr);
    const int scheduleStartAddr = 0; // Starting address for the ring schedule IS THIS CORRECT?
    const int ringDurationStartAddr = 200;
    const int deviceNameAddr = 400;
    const int passwordAddr = 600;
    // Define more addresses as needed
};

#endif

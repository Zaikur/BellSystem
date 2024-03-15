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
    void begin(size_t size);
    bool saveRingSchedule(const String& schedule);
    bool wipeSavedSchedule();
    bool saveRingDuration(int duration);
    bool wipeSavedRingDuration();
    int loadRingDuration();
    bool saveDeviceName(const String& deviceName);
    String loadDeviceName();
    // Additional functionalities as needed

private:
    size_t findEEPROMEnd();
    bool saveString(const String& data, int startAddr);
    String loadString(int startAddr, int maxLen);
    bool saveInt(int value, int startAddr);
    int loadInt(int startAddr);
    const int scheduleStartAddr = 0; // Starting address for the ring schedule
    const int ringDurationStartAddr = 200; // Example address for WiFi credentials
    const int deviceNameAddr = 400; // Example address for device name
    // Define more addresses as needed
};

#endif

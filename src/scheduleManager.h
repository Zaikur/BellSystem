/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include "TimeManager.h"
#include "RelayManager.h"
#include "EEPROMLayoutManager.h"

class ScheduleManager {
    private:
        EEPROMLayoutManager eepromManager;
        TimeManager timeManager;
        RelayManager relayManager;
    public:
        ScheduleManager();
        bool saveSchedule(const String& jsonSchedule);
        String getScheduleString();
        DynamicJsonDocument getScheduleJson();
        void handleRing();
    private:
        bool shouldRingNow();
        String dayOfWeekStr(int day);
        void loadScheduleFromEEPROM();
        DynamicJsonDocument currentSchedule;
        int duration;
};
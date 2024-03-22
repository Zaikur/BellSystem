/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include "TimeManager.h"
#include "../board/RelayManager.h"
#include "../board/EEPROMLayoutManager.h"

// Global objects initialized in main.cpp
extern EEPROMLayoutManager eepromManager;
extern TimeManager timeManager;
extern RelayManager relayManager;

class ScheduleManager {
    public:
        ScheduleManager();
        bool saveSchedule(const String& jsonSchedule);
        String getScheduleString();
        String getTodayRemainingRingTimes();
        DynamicJsonDocument getScheduleJson();
        void handleRing();
        void updateSchedule(const String& jsonSchedule);
    private:
        bool shouldRingNow();
        String dayOfWeekStr(int day);
        void loadScheduleFromEEPROM();
        DynamicJsonDocument currentSchedule;
};
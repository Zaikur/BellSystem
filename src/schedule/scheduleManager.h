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
        String getScheduleString();
        String getTodayRemainingRingTimes();
        void handleRing();
        bool updateSchedule(const String& jsonSchedule);
    private:
        bool shouldRingNow();
        JsonArray getRemainingRingTimes();
        void sortScheduleTimes(DynamicJsonDocument& schedule);
        String dayOfWeekStr(int day);
        void loadScheduleFromEEPROM();
        DynamicJsonDocument currentSchedule;
        bool validateSchedule(DynamicJsonDocument& schedule);
        bool isValidTimeFormat(const String& time);
};
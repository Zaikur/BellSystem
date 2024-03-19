/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include "scheduleManager.h"

ScheduleManager::ScheduleManager() : eepromManager(), timeManager(), relayManager(5) {
    eepromManager.begin(2048);
}

bool ScheduleManager::saveSchedule(const String& jsonSchedule) {
    //ADD VALIDATION FOR JSON STRING**********************************************************
    return eepromManager.saveRingSchedule(jsonSchedule);
}

String ScheduleManager::getSchedule() {
    return eepromManager.loadRingSchedule();
}

void ScheduleManager::handleRing() {
    if (shouldRingNow()) {
        int duration = eepromManager.loadRingDuration();
        relayManager.activateRelay(duration);
    }
}

bool ScheduleManager::shouldRingNow() {
    String schedule = eepromManager.loadRingSchedule();
    if (schedule.length() == 0) {
        return false;
    }

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, schedule);
    
    JsonObject root = doc.as<JsonObject>();
    String dayOfWeek = dayOfWeekStr(timeManager.getDayOfWeek());
    if (root.containsKey(dayOfWeek)) {
        JsonArray times = root[dayOfWeek].as<JsonArray>();
        for (size_t i = 0; i < times.size(); i++) {
            String time = times[i].as<String>();
            if (timeManager.getTime() == time) {
                return true;
            }
        }
    }
    return false;
}

String ScheduleManager::dayOfWeekStr(int day) {
    switch (day) {
        case 1:
            return "sunday";
        case 2:
            return "monday";
        case 3:
            return "tuesday";
        case 4:
            return "wednesday";
        case 5:
            return "thursday";
        case 6:
            return "friday";
        case 7:
            return "saturday";
        default:
            return "";
    }
}
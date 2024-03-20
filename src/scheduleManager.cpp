/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include "scheduleManager.h"

/****************PUBLIC******************/


// Constructor for ScheduleManager class that initializes the EEPROMLayoutManager, TimeManager, and RelayManager objects
ScheduleManager::ScheduleManager() : eepromManager(), timeManager(), relayManager(5), currentSchedule(1024) {
    eepromManager.begin(2048);
    loadScheduleFromEEPROM();
    duration = eepromManager.loadRingDuration();
}

// This method saves the schedule to EEPROM and updates the currentSchedule object
bool ScheduleManager::saveSchedule(const String& jsonSchedule) {
    deserializeJson(currentSchedule, jsonSchedule);
    return eepromManager.saveRingSchedule(jsonSchedule);
}

// This method returns the current schedule as a string
String ScheduleManager::getScheduleString() {
    return currentSchedule.as<String>();
}

// This method returns the current schedule as a JSON object
DynamicJsonDocument ScheduleManager::getScheduleJson() {
    return currentSchedule;
}

// This method checks if the bell should ring and activates the relay if it should
void ScheduleManager::handleRing() {
    if (shouldRingNow()) {
        relayManager.activateRelay(duration);
    }
}

/****************PRIVATE******************/

// This method checks if the bell should ring at the current time
bool ScheduleManager::shouldRingNow() {
    JsonObject root = currentSchedule.as<JsonObject>(); // Use currentSchedule
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

// This method returns the day of the week as a string
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

// This method loads the current schedule from EEPROM to be stored in memory when the device starts
void ScheduleManager::loadScheduleFromEEPROM() {
    String schedule = eepromManager.loadRingSchedule();
    if (schedule.length() > 0 && schedule[0] != char(0xFF)) {
        deserializeJson(currentSchedule, schedule);
    }
}
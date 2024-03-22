/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include "scheduleManager.h"

/****************PUBLIC******************/

// Constructor for ScheduleManager class that initializes the EEPROMLayoutManager, TimeManager, and RelayManager objects
ScheduleManager::ScheduleManager() : currentSchedule(1024) {
    eepromManager.begin(2048);
    loadScheduleFromEEPROM();
}

// This method updates the currentSchedule object
void ScheduleManager::updateSchedule(const String& jsonSchedule) {
    deserializeJson(currentSchedule, jsonSchedule);
}

// This method saves the schedule to EEPROM and updates the currentSchedule object
bool ScheduleManager::saveSchedule(const String& jsonSchedule) {
    deserializeJson(currentSchedule, jsonSchedule);
    return eepromManager.saveRingSchedule(jsonSchedule);
}

// This method returns a list of the remaining ring times for today
String ScheduleManager::getTodayRemainingRingTimes() {
    String result = "";
    String now = timeManager.getDateTime(); // Returns "YYYY-MM-DD HH:MM:SS"
    String currentTime = now.substring(11, 16); // Extract "HH:MM"

    int dayOfWeek = timeManager.getDayOfWeek(); // 1=Sunday, 7=Saturday
    String today = dayOfWeekStr(dayOfWeek);

    // Check if today's schedule exists
    if (currentSchedule.containsKey(today)) {
        JsonArray timesArray = currentSchedule[today].as<JsonArray>();

        // Loop through today's ring times
        for (JsonVariant v : timesArray) {
            String ringTime = v.as<String>();
            if (ringTime > currentTime) {
                // This ring time is still upcoming today
                if (!result.isEmpty()) {
                    result += ","; // Separate times with a comma if not the first entry
                }
                result += ringTime; // Append the upcoming ring time
            }
        }
    }

    if (result.isEmpty()) {
        return "No more rings today";
    } else {
        return result; // Return the concatenated string of remaining ring times
    }
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
    Serial.println("Checking if the bell should ring");
    if (shouldRingNow()) {
        Serial.println("Ringing the bell");
        relayManager.activateRelay();
    }
}

/****************PRIVATE******************/

// This method checks if the bell should ring at the current time
bool ScheduleManager::shouldRingNow() {
    JsonObject root = currentSchedule.as<JsonObject>(); // Use currentSchedule
    String dayOfWeek = dayOfWeekStr(timeManager.getDayOfWeek());
    String currentTime = timeManager.getTime(); // Get the current time once for comparison

    if (root.containsKey(dayOfWeek)) {
        JsonArray times = root[dayOfWeek].as<JsonArray>();

        Serial.print("Checking times for ");
        Serial.println(dayOfWeek);

        for (JsonVariant timeVariant : times) {
            String scheduledTime = timeVariant.as<String>(); // Get the scheduled time as a String
            Serial.println(scheduledTime);

            if (currentTime == scheduledTime) {
                Serial.println("Time to ring the bell.");
                return true; // It's time to ring the bell
            }
        }
    }
    return false; // It's not time to ring the bell
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
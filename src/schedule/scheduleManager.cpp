/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include "scheduleManager.h"

/****************PUBLIC******************/

// Constructor for ScheduleManager class that initializes the EEPROMLayoutManager, TimeManager, and RelayManager objects
ScheduleManager::ScheduleManager() : currentSchedule(1024) {
    loadScheduleFromEEPROM();
}

// This method saves the schedule to EEPROM and updates the currentSchedule object
// Returns true if the schedule was saved successfully
bool ScheduleManager::updateSchedule(const String& jsonSchedule) {
    DynamicJsonDocument tempSchedule(1024); // Temp schedule object (used for sorting times)

    // Deserialize the JSON string into the tempSchedule object
    auto error = deserializeJson(tempSchedule, jsonSchedule);
    if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return false; // Indicate failure to update schedule
    }

    
    // Validate the schedule structure and content
    if (!validateSchedule(tempSchedule)) {
        Serial.println("Schedule validation failed.");
        return false; // Schedule is not as expected, indicate failure
    }

    // Sort the schedule times for each day (modified by reference)
    sortScheduleTimes(tempSchedule);

    // Clear the currentSchedule object and copy the tempSchedule object to it
    currentSchedule.clear();
    currentSchedule = tempSchedule;

    // Now that currentSchedule is updated, save back to EEPROM
    String updatedJsonString;
    serializeJson(currentSchedule, updatedJsonString); // Serialize the currentSchedule object back to a string
    return eepromManager.saveRingSchedule(updatedJsonString); // Save the updated schedule to EEPROM
}


// This method returns the current schedule as a string
String ScheduleManager::getScheduleString() {
    return currentSchedule.as<String>();
}

// This method checks if the bell should ring and activates the relay if it should
void ScheduleManager::handleRing() {
    Serial.println("Checking if the bell should ring");
    if (shouldRingNow()) {
        Serial.println("Ringing the bell");
        relayManager.activateRelay();
    }
}

// This method checks ring times for the current day
// Returns an array of times for the current day that are still upcoming
String ScheduleManager::getTodayRemainingRingTimes() {
    JsonArray remainingTimes = getRemainingRingTimes();
    String result;
    for (JsonVariant v : remainingTimes) {
        if (!result.isEmpty()) {
            result += ",";
        }
        result += v.as<String>();
    }
    return result.isEmpty() ? "No more rings today" : result;
}

/****************PRIVATE******************/

// This method checks if the bell should ring at the current time
// Returns true if the bell should ring now
bool ScheduleManager::shouldRingNow() {
    String currentTime = timeManager.getTime(); // "HH:MM" format
    String today = dayOfWeekStr(timeManager.getDayOfWeek());
    
    // Ensure currentSchedule contains today's schedule
    if (!currentSchedule.containsKey(today)) return false;

    JsonArray times = currentSchedule[today].as<JsonArray>();

    // Look for the exact current time in today's schedule
    for (JsonVariant v : times) {
        if (currentTime == v.as<String>()) {
            return true; // Time to ring the bell
        }
    }
    return false; // No ring scheduled for the current minute
}


// This method gets the current time, day of the week, and compares it to the schedule
// Returns an array of times for the current day that are still upcoming
JsonArray ScheduleManager::getRemainingRingTimes() {
    String dayOfWeek = dayOfWeekStr(timeManager.getDayOfWeek());
    String currentTime = timeManager.getTime();
    JsonArray result = currentSchedule.createNestedArray("temp"); // Temp array for results

    if (currentSchedule.containsKey(dayOfWeek)) {
        JsonArray timesArray = currentSchedule[dayOfWeek].as<JsonArray>();
        for (JsonVariant v : timesArray) {
            String ringTime = v.as<String>();
            if (ringTime > currentTime) {
                result.add(ringTime);
            }
        }
    }

    return result;
}

// This method sorts the schedule times for each day in the schedule, modifying the schedule by reference
// Each day (key) in the schedule, has an associated JsonArray of times that are sorted
// A temporary std::vector is created for each day, allowing for easy sorting
// The original JsonArray is cleared and repopulated with the sorted times
void ScheduleManager::sortScheduleTimes(DynamicJsonDocument& schedule) {
    for (JsonPair kv : schedule.as<JsonObject>()) {
        String day = kv.key().c_str();
        JsonArray times = kv.value().as<JsonArray>();

        // Create a temporary std::vector for sorting
        std::vector<String> sortedTimes;
        for (JsonVariant v : times) {
            sortedTimes.push_back(v.as<String>());
        }

        // Sort the std::vector
        std::sort(sortedTimes.begin(), sortedTimes.end());

        // Clear the original JsonArray and repopulate it with sorted times
        times.clear();
        for (const String& time : sortedTimes) {
            times.add(time);
        }
    }
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

// This method ensures that the received schedule is as expected
bool ScheduleManager::validateSchedule(DynamicJsonDocument& schedule) {
    const char* daysOfWeek[] = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
    
    for (const char* day : daysOfWeek) {
        // Check if the key for the day exists in the schedule
        if (!schedule.containsKey(day)) continue; // It's okay if a day doesn't have a schedule
        
        JsonArray times = schedule[day].as<JsonArray>();
        if (!times) {
            Serial.print(day);
            Serial.println(" is not an array of times.");
            return false; // Day exists but is not an array, structure is invalid
        }

        // Validate each time string format (optional, if you want to ensure HH:MM format)
        for (JsonVariant v : times) {
            String time = v.as<String>();
            if (!isValidTimeFormat(time)) {
                Serial.print(day);
                Serial.print(": ");
                Serial.print(time);
                Serial.println(" is not a valid time.");
                return false; // Time format is invalid
            }
        }
    }
    return true; // Passed all checks
}

// Utility function to validate time format (HH:MM)
bool ScheduleManager::isValidTimeFormat(const String& time) {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;
    int hour = time.substring(0, 2).toInt();
    int minute = time.substring(3, 5).toInt();
    return hour >= 0 && hour < 24 && minute >= 0 && minute < 60;
}

// This method loads the current schedule from EEPROM to be stored in memory when the device starts
void ScheduleManager::loadScheduleFromEEPROM() {
    String schedule = eepromManager.loadRingSchedule();
    Serial.println("Loaded schedule from EEPROM: " + schedule);
    if (schedule.length() > 0 && schedule[0] != char(0xFF)) {
        // Clear the current schedule object
        currentSchedule.clear();

        // Deserialize the JSON string into the currentSchedule object
        deserializeJson(currentSchedule, schedule);
        Serial.println("Schedule loaded successfully");
        Serial.println(currentSchedule.as<String>());
    }
}
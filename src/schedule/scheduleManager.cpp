/*
Quinton Nelson
3/22/2024
This file handles reading a schedule from the client, saving it to EEPROM, and checking if the bell should ring
*/

#include "scheduleManager.h"

/****************PUBLIC******************/

// Constructor for ScheduleManager class that initializes the EEPROMLayoutManager, TimeManager, and RelayManager objects
ScheduleManager::ScheduleManager() : currentSchedule(4096) {
    loadScheduleFromEEPROM();
    ringInterval = 60000; // 1 minute interval
    lastRingTimeMillies = 0;
}


/**
 * The function `updateSchedule` in the `ScheduleManager` class updates and saves a schedule by
 * deserializing, validating, sorting, and serializing JSON data.
 * 
 * @param jsonSchedule The `jsonSchedule` parameter in the `updateSchedule` function is a JSON string
 * that represents a schedule. This JSON string is deserialized into a `DynamicJsonDocument` object
 * named `tempSchedule` for further processing. The function then validates the structure and content
 * of the schedule, sorts the schedule
 * 
 * @return The `updateSchedule` function returns a boolean value. It returns `true` if the schedule
 * update was successful and the updated schedule was saved to EEPROM, and it returns `false` if there
 * was an error during the process such as failure to deserialize the JSON string, validation failure,
 * or failure to save the updated schedule to EEPROM.
 */
bool ScheduleManager::updateSchedule(const String& jsonSchedule) {
    DynamicJsonDocument tempSchedule(4096); // Temp schedule object (used for sorting times)

    // Deserialize the JSON string into the tempSchedule object
    auto error = deserializeJson(tempSchedule, jsonSchedule);
        if (error) {
        return false; // Indicate failure to update schedule
    }
    
    
    // Validate the schedule structure and content
    if (!validateSchedule(tempSchedule)) {
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


/**
 * The function `getScheduleString` returns a JSON string representation of the current schedule.
 * 
 * @return The `getScheduleString` function returns a string representation of the `currentSchedule`
 * object after serializing it using the `serializeJson` function.
 */
String ScheduleManager::getScheduleString() {
    String schedule;
    serializeJson(currentSchedule, schedule);
    return schedule;
}

/**
 * The `handleRing` function checks if the bell should ring and activates the relay if necessary.
 */
void ScheduleManager::handleRing() {
    unsigned long currentMillis = millis();
    if (shouldRingNow() && (currentMillis - lastRingTimeMillies) >= ringInterval) {
        relayManager.activateRelay();
        lastRingTimeMillies = currentMillis;
    }
}

/**
 * The function `getTodayRemainingRingTimes` returns a string containing the remaining ring times for
 * today in a comma-separated format or "No more rings today" if there are none.
 * 
 * @return The `getTodayRemainingRingTimes` function returns a string that contains the remaining ring
 * times for today. If there are no more ring times remaining for the day, it returns the string "No
 * more rings today".
 */
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

/**
 * The function `shouldRingNow` checks if the current time matches any scheduled time for ringing a
 * bell based on the day of the week.
 * 
 * @return The function `shouldRingNow()` returns a boolean value. It returns `true` if the current
 * time matches any of the scheduled times for the day in the `currentSchedule`, indicating that the
 * bell should ring now. Otherwise, it returns `false`, indicating that there is no scheduled ringing
 * for the current minute.
 */
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


/**
 * The function `getRemainingRingTimes` retrieves the remaining ring times for the current day based on
 * the current time.
 * 
 * @return A JsonArray containing the remaining ring times for the current day that are after the
 * current time.
 */
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


/**
 * The function `sortScheduleTimes` sorts the times in each day's schedule in ascending order.
 * 
 * @param schedule The `sortScheduleTimes` function takes a `DynamicJsonDocument` reference named
 * `schedule` as a parameter. This `schedule` parameter is expected to contain a JSON object
 * representing a schedule with days as keys and an array of times as values. The function iterates
 * over each key-value pair in
 */
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


/**
 * The function `dayOfWeekStr` converts a numerical day of the week to its corresponding string
 * representation.
 * 
 * @param day The `dayOfWeekStr` function takes an integer `day` as input and returns a string
 * representing the day of the week corresponding to that integer. The mapping is as follows:
 * 
 * @return The `dayOfWeekStr` function takes an integer `day` as input and returns a string
 * representing the day of the week corresponding to that integer. The function uses a switch statement
 * to match the input `day` with the corresponding day of the week and returns the string
 * representation of that day. If the input `day` does not match any of the cases (1 to 7), an empty
 */
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


/**
 * The function `validateSchedule` checks if a given schedule JSON object contains valid time entries
 * for each day of the week.
 * 
 * @param schedule The `validateSchedule` function takes a `DynamicJsonDocument` object named
 * `schedule` as a parameter. This object is expected to contain schedules for each day of the week,
 * where the keys are the names of the days ("monday", "tuesday", etc.) and the corresponding values
 * are
 * 
 * @return The `validateSchedule` function returns a boolean value. It returns `true` if the schedule
 * passes all validation checks, and `false` if any validation check fails during the iteration over
 * the days of the week and their corresponding times.
 */
bool ScheduleManager::validateSchedule(DynamicJsonDocument& schedule) {
    const char* daysOfWeek[] = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
    
    for (const char* day : daysOfWeek) {
        // Check if the key for the day exists in the schedule
        if (!schedule.containsKey(day)) continue; // It's okay if a day doesn't have a schedule

        JsonArray times = schedule[day].as<JsonArray>();
        if (!times) {
            return false; // Day exists but is not an array, structure is invalid
        }

        // Validate each time string format (optional, if you want to ensure HH:MM format)
        for (JsonVariant v : times) {
            String time = v.as<String>();
            if (!isValidTimeFormat(time)) {
                return false; // Time format is invalid
            }
        }
    }
    return true; // Passed all checks
}


/**
 * The function `isValidTimeFormat` in C++ checks if a given time string is in a valid format (HH:MM).
 * 
 * @param time The `isValidTimeFormat` function checks if the input time string is in a valid format
 * for a 24-hour clock. The function first checks if the length of the time string is 5 characters and
 * if the character at index 2 is a colon ':'.
 * 
 * @return The `isValidTimeFormat` function returns a boolean value indicating whether the input time
 * string has a valid format for representing a time (HH:MM).
 */
bool ScheduleManager::isValidTimeFormat(const String& time) {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;
    int hour = time.substring(0, 2).toInt();
    int minute = time.substring(3, 5).toInt();
    return hour >= 0 && hour < 24 && minute >= 0 && minute < 60;
}


/**
 * The function `loadScheduleFromEEPROM` loads a ring schedule from EEPROM, deserializes it into a JSON
 * object, and prints the loaded schedule.
 */
void ScheduleManager::loadScheduleFromEEPROM() {
    String schedule = eepromManager.loadRingSchedule();

    if (schedule.length() > 0 && schedule[0] != char(0xFF)) {
        // Clear the current schedule object
        currentSchedule.clear();

        // Deserialize the JSON string into the currentSchedule object
        deserializeJson(currentSchedule, schedule);
    }
}
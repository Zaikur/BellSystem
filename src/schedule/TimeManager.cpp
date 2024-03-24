/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and returning the current time and date
*/

#include "TimeManager.h"

TimeManager::TimeManager() {}

/**
 * The `begin` function in the `TimeManager` class sets the time zone to CST and waits for time
 * synchronization from an NTP server.
 */
void TimeManager::begin() {
    // Set the time zone to CST, using Posix format which contains the daylight savings time rules
    myTimeZone.setPosix("CST6CDT,M3.2.0,M11.1.0");

    //This function waits for the time to be synchronized from the NTP server
    if (waitForSync()) {
        eepromManager.addSystemMessage("Time synchronized successfully.");
    } else {
        eepromManager.addSystemMessage("Time synchronization failed.");
    }
}

/**
 * The getTime function in the TimeManager class retrieves the current hour and minute, formats them
 * with leading zeros if necessary, and returns the time in the format "HH:MM".
 * 
 * @return The `getTime()` function returns a formatted string representing the current time in the
 * format "HH:MM" (hours:minutes). The function first retrieves the current hour and minute in the
 * local time zone, then formats them with leading zeros if necessary, and finally returns the
 * formatted time string.
 */
String TimeManager::getTime() {
    // Get the current hour and minute
    String hour = String(myTimeZone.hour());
    String minute = String(myTimeZone.minute());

    // Add a leading zero if the hour or minute is less than 10
    if (hour.length() == 1) hour = "0" + hour;
    if (minute.length() == 1) minute = "0" + minute;

    String time = hour + ":" + minute;
    return time;
}

/**
 * The getDateTime function in the TimeManager class retrieves and returns the current date and time in
 * the format "Y-m-d H:i:s".
 * 
 * @return The `getDateTime` function returns a string containing the current date and time in the
 * format "Y-m-d H:i:s".
 */
String TimeManager::getDateTime() {
    // Get the current date and time
    String dateTime = myTimeZone.dateTime("Y-m-d H:i:s");
    return dateTime;
}

/**
 * The function `getDayOfWeek` in the `TimeManager` class returns the current day of the week.
 * 
 * @return The function `getDayOfWeek()` is returning the day of the week as an integer value.
 */
int TimeManager::getDayOfWeek() {
    time_t now = myTimeZone.now();
    int dayOfWeek = weekday(now);
    return dayOfWeek;
}
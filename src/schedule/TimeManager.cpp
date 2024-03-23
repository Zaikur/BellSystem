/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and returning the current time and date
*/

#include "TimeManager.h"

// -6 * 3600 is the time zone offset for CST (Not in daylight savings time)
// 60000 is the update interval in milliseconds
TimeManager::TimeManager() {}

// This method initializes the ezTime library and waits for the time to be synchronized
void TimeManager::begin() {
    // Set the time zone to CST
    //if (!myTimeZone.setCache(100)) 
    if (!myTimeZone.setCache(700)) myTimeZone.setLocation("America/Chicago");

    //This function waits for the time to be synchronized from the NTP server
    waitForSync();

    // Print the current time to the serial monitor
    Serial.println("Time: " + myTimeZone.dateTime("Y-m-d H:i:s"));
}

// This method returns the current time in HH:MM format
String TimeManager::getTime() {
    // Get the current hour and minute
    String hour = String(myTimeZone.hour());
    String minute = String(myTimeZone.minute());

    // Add a leading zero if the hour or minute is less than 10
    if (hour.length() == 1) hour = "0" + hour;
    if (minute.length() == 1) minute = "0" + minute;

    String time = hour + ":" + minute;
    Serial.println("Time: " + time);
    return time;
}

// This method returns the current dateTime in YYYY-MM-DD HH:MM:SS format
// Include the necessary header for time functions
String TimeManager::getDateTime() {
    // Get the current date and time
    String dateTime = myTimeZone.dateTime("Y-m-d H:i:s");
    Serial.println("Date Time: " + dateTime);
    return dateTime;
}

// This method returns day of the week (1 = Sunday, 7 = Saturday) 
int TimeManager::getDayOfWeek() {
    time_t now = myTimeZone.now();
    int dayOfWeek = weekday(now);
    Serial.println("Day of week: " + dayOfWeek);
    return dayOfWeek;
}
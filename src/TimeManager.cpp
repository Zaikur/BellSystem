/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and calculating next scheduled bell ring
*/

#include "TimeManager.h"

TimeManager::TimeManager() : timeClient(ntpUDP, "pool.ntp.org", 0, 60000) {}


// This method returns the current time in HH:MM format
String TimeManager::getTime() {
    // Returns time in HH:MM format
    String hour = String(timeClient.getHours());
    String minute = String(timeClient.getMinutes());

    // Pad with 0 if necessary
    if(hour.length() == 1) hour = "0" + hour;
    if(minute.length() == 1) minute = "0" + minute;

    return hour + ":" + minute;
}

// This method returns the current dateTime in YYYY-MM-DD HH:MM:SS format
// Include the necessary header for time functions
String TimeManager::getDateTime() {
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);

    char dateTime[24]; // Slightly larger buffer for safety
    snprintf(dateTime, sizeof(dateTime), "%04d-%02d-%02d %02d:%02d:%02d",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    return String(dateTime);
}



// This method gets the current time from the NTP server as UNIX time (number of seconds since Jan 1 1970)
// Returns day of the week (1 = Sunday, 7 = Saturday) Using the weekday() function from TimeLib.h
int TimeManager::getDayOfWeek() {
    time_t now = timeClient.getEpochTime();
    return weekday(now);
}
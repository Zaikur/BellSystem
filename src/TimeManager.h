/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and calculating next scheduled bell ring
*/

#ifndef TimeManager_h
#define TimeManager_h

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <time.h>

class TimeManager {
public:
    TimeManager();
    String getTime();
    String getDateTime();
    int getDayOfWeek();
private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
};

#endif

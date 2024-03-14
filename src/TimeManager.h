/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and calculating next scheduled bell ring
*/

#ifndef TimeManager_h
#define TimeManager_h

#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeManager {
public:
    TimeManager();
    void updateTime();
private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
};

#endif

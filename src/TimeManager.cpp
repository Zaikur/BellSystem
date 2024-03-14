/*
Quinton Nelson
3/12/2024
This file handles time synchronization with NTP, keeping track of the current time, and calculating next scheduled bell ring
*/

#include "TimeManager.h"

TimeManager::TimeManager() : timeClient(ntpUDP, "pool.ntp.org", 0, 60000) {}

void TimeManager::updateTime() {
    timeClient.begin();
    timeClient.setTimeOffset(-5 * 3600); // Set your timezone offset here
    timeClient.update();
    Serial.print("Current time: ");
    Serial.println(timeClient.getFormattedTime());
}
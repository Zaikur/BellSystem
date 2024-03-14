#ifndef RelayManager_h
#define RelayManager_h

#include <Arduino.h>

class RelayManager {
public:
    RelayManager(int pin);
    void activateRelay(int duration);
private:
    int relayPin;
};

#endif

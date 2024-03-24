/*
Quinton Nelson
3/15/2024
This file handles EEPROM management
Contains methods to save and load data from EEPROM including ring schedule, ring duration, and device name
Start address for each data type is defined as a constant, and methods are provided to save and load data from these addresses
*/


#include "EEPROMLayoutManager.h"

EEPROMLayoutManager::EEPROMLayoutManager() {}

bool EEPROMLayoutManager::begin(size_t size) {
    EEPROM.begin(size);
    return true;
}

/****************************System messages****************************/

/**
 * The function `addSystemMessage` adds a new message to an array, removing the first element if the
 * array is already full.
 * 
 * @param message The `message` parameter is a pointer to a constant character array (string) that
 * contains the system message to be added to the `systemMessages` array in the `EEPROMLayoutManager`
 * class.
 */
void EEPROMLayoutManager::addSystemMessage(const char* message) {
    // Directly access the JsonArray within systemMessages
    JsonArray messagesArray = systemMessages.as<JsonArray>();

    // Check if the array is full
    if (messagesArray.size() >= 15) {
        // Remove the first element
        messagesArray.remove(0);
    }

    // Add the new message
    messagesArray.add(message);
}


/****************************Ring schedule****************************/
/**
 * The function `saveRingSchedule` saves a string schedule to EEPROM memory and returns a boolean
 * indicating the success of the save operation.
 * 
 * @param schedule The `schedule` parameter is a `String` object that contains the ring schedule data
 * to be saved in the EEPROM.
 * 
 * @return The function `saveRingSchedule` is returning a boolean value indicating whether the saving
 * of the ring schedule was successful or not.
 */
bool EEPROMLayoutManager::saveRingSchedule(const String& schedule) {
    bool saveSuccess = saveString(schedule, scheduleStartAddr);
    return saveSuccess;
}

/**
 * The function `loadRingSchedule` loads a schedule from EEPROM and returns it, or an empty string if
 * the schedule is empty.
 * 
 * @return The function `loadRingSchedule()` returns either an empty string if the loaded schedule is
 * empty or the loaded schedule itself if it is not empty.
 */
String EEPROMLayoutManager::loadRingSchedule() {
    String schedule = loadString(scheduleStartAddr, 1000);
    if (schedule.length() > 0 && schedule[0] == char(0xFF)) {
        return ""; // Return an empty string if the schedule is empty
    } else {
        return schedule; // Return the loaded schedule
    }
}

/****************************Ring duration****************************/
/**
 * The function `saveRingDuration` saves an integer value representing ring duration to EEPROM memory.
 * 
 * @param duration Duration of the ring in seconds.
 * 
 * @return The function `saveRingDuration` is returning the result of calling the `saveInt` function
 * with the `duration` parameter and the `ringDurationStartAddr` as arguments.
 */
bool EEPROMLayoutManager::saveRingDuration(int duration) {
    return saveInt(duration, ringDurationStartAddr);
}

/**
 * The function `loadRingDuration` loads a ring duration value from EEPROM, with a default value of 2
 * if the loaded value is 0 or outside the range of 0 to 10.
 * 
 * @return The function `loadRingDuration` will return the loaded ring duration if it is between 0 and
 * 10 (inclusive). If the loaded duration is 0 or outside the range of 0 to 10, it will return the
 * default ring duration of 2.
 */
int EEPROMLayoutManager::loadRingDuration() {
    int duration = loadInt(ringDurationStartAddr);
    if (duration == 0) {
        return 2; // Return the default ring duration
    } else if (duration > 10 || duration < 0) {
        return 2;
    } else {
        return duration; // Return the loaded ring duration
    }
}

/****************************Device settings****************************/
/**
 * The function `saveDeviceName` saves a device name to EEPROM memory.
 * 
 * @param deviceName A string containing the name of the device that needs to be saved in the EEPROM.
 * 
 * @return The `saveDeviceName` function is returning the result of calling the `saveString` function
 * with the `deviceName` parameter and `deviceNameAddr` address.
 */
bool EEPROMLayoutManager::saveDeviceName(const String& deviceName) {
    return saveString(deviceName, deviceNameAddr);
}

/**
 * The function `loadDeviceName` loads a device name from EEPROM and returns either the loaded name or
 * a default name if the loaded name starts with a specific character.
 * 
 * @return The function `loadDeviceName()` returns either the default device name "bellsystem" if the
 * loaded device name starts with the character 0xFF, or it returns the loaded device name if it is not
 * empty and does not start with 0xFF.
 */
String EEPROMLayoutManager::loadDeviceName() {
    String deviceName = loadString(deviceNameAddr, 100);
    if (deviceName.length() > 0 && deviceName[0] == char(0xFF)) {
        return "bellsystem"; // Return the default device name
    } else {
        return deviceName; // Return the loaded device name
    }
}

/*****************************Unique URL******************************/
/**
 * The function `loadUniqueURL` loads a unique URL from EEPROM memory, returning a default device name
 * if the URL starts with 0xFF.
 * 
 * @return The function `loadUniqueURL()` returns a String value. If the loaded URL starts with the
 * character 0xFF, it returns "bellsystem" as the default device name. Otherwise, it returns the loaded
 * device name.
 */
String EEPROMLayoutManager::loadUniqueURL() {
    String url = loadString(uniqueURLAddr, 100);
    if (url.length() > 0 && url[0] == char(0xFF)) {
        return "bellsystem"; // Return the default device name
    } else {
        return url; // Return the loaded device name
    }
}

/**
 * The function `saveUniqueURL` saves a unique URL string to EEPROM memory.
 * 
 * @param url A string containing the URL that needs to be saved in the EEPROM.
 * 
 * @return The function `saveUniqueURL` is returning the result of calling the `saveString` function
 * with the `url` parameter and the `uniqueURLAddr` address.
 */
bool EEPROMLayoutManager::saveUniqueURL(const String& url) {
    return saveString(url, uniqueURLAddr);
}

/*****************************Password******************************/
/**
 * The function `savePassword` saves a password string to EEPROM memory.
 * 
 * @param password The `password` parameter is a `String` object that contains the password to be saved
 * in the EEPROM.
 * 
 * @return The `savePassword` function is returning the result of calling the `saveString` function
 * with the `password` parameter and `passwordAddr` address as arguments.
 */
bool EEPROMLayoutManager::savePassword(const String& password) {
    return saveString(password, passwordAddr);
}

/**
 * The function `loadPassword` loads a password from EEPROM, returning a default password if the stored
 * password is empty or corrupted.
 * 
 * @return The function `loadPassword()` returns either the default password "password" if the loaded
 * password starts with the character 0xFF, or it returns the loaded password from the EEPROM.
 */
String EEPROMLayoutManager::loadPassword() {
    String password =  loadString(passwordAddr, 100);
    if (password.length() > 0 && password[0] == char(0xFF)) {
        return "password"; // Return the default password
    } else {
        return password; // Return the loaded password
    }
}

/**
 * The function `saveSalt` saves a string to EEPROM at a specific address.
 * 
 * @param salt The `salt` parameter is a `String` object that contains the salt value to be saved in
 * the EEPROM.
 * 
 * @return The `saveSalt` function is returning the result of calling the `saveString` function with
 * the `salt` parameter and `saltAddr` address.
 */
bool EEPROMLayoutManager::saveSalt(const String& salt) {
    return saveString(salt, saltAddr);
}

/**
 * The function `loadSalt` loads a string from EEPROM at a specific address and returns it.
 * 
 * @return The `loadSalt()` function is returning a String variable named `salt` which contains the
 * value loaded from the EEPROM at address `saltAddr`.
 */
String EEPROMLayoutManager::loadSalt() {
    String salt = loadString(saltAddr, 100);
    return salt;
}

/**
 * The function `saveInitialized` class saves an integer value
 * representing the boolean `initialized` to a specific address in EEPROM.
 * 
 * @param initialized The `initialized` parameter is a boolean value that indicates whether a certain
 * operation or process has been initialized or not.
 * 
 * @return The function `EEPROMLayoutManager::saveInitialized` is returning the result of calling the
 * `saveInt` function with the value `initialized ? 1 : 0` and the `initializedAddr` as arguments.
 */
bool EEPROMLayoutManager::saveInitialized(bool initialized) {
    return saveInt(initialized ? 1 : 0, initializedAddr);
}

/**
 * The function `loadInitialized` in the `EEPROMLayoutManager` class loads an integer value from EEPROM
 * and returns true if the value is 1.
 * 
 * @return The function `loadInitialized()` returns a boolean value, true if the stored value in the
 * EEPROM is 1, and false otherwise.
 */
bool EEPROMLayoutManager::loadInitialized() {
    int initialized = loadInt(initializedAddr);
    return initialized == 1;  // Return true if stored value is 1
}

/****************************Private methods****************************/


/**
 * The function `saveString` saves a string to EEPROM memory with null termination.
 * 
 * @param data The `data` parameter is a reference to a `String` object that contains the string data
 * to be saved in the EEPROM memory.
 * @param startAddr The `startAddr` parameter in the `saveString` function represents the starting
 * address in the EEPROM (Electrically Erasable Programmable Read-Only Memory) where the string data
 * will be saved. The function iterates over the characters in the input string `data` and writes each
 * character to
 * 
 * @return The function `saveString` returns a boolean value, which is the result of calling
 * `EEPROM.commit()`.
 */
bool EEPROMLayoutManager::saveString(const String& data, int startAddr) {
    unsigned int i;
    for (i = 0; i < data.length(); i++) {
        EEPROM.write(startAddr + i, data[i]);
    }
    EEPROM.write(startAddr + i, '\0'); // Null-terminate the string
    return EEPROM.commit();
}

/**
 * The function `loadString` reads a string from EEPROM starting at a specified address and ending at a
 * null terminator or maximum length, then prints and returns the loaded string.
 * 
 * @param startAddr The `startAddr` parameter specifies the starting address in the EEPROM from which
 * the string should be loaded.
 * @param maxLen The `maxLen` parameter in the `loadString` function represents the maximum length of
 * the string that is being loaded from the EEPROM starting at the specified address `startAddr`. This
 * parameter is used to limit the number of characters read from the EEPROM to prevent reading beyond
 * the allocated space for the string
 * 
 * @return The function `loadString` returns a `String` object that contains the string loaded from the
 * EEPROM starting at the specified address `startAddr` with a maximum length of `maxLen`.
 */
String EEPROMLayoutManager::loadString(int startAddr, int maxLen) {
    String result = "";
    for (int i = 0; i < maxLen; i++) {
        char ch = EEPROM.read(startAddr + i);
        if (ch == '\0') break;
        result += ch;
    }
    return result;
}

/**
 * The function `saveInt` saves an integer value to EEPROM memory starting from a specified address.
 * 
 * @param value The `value` parameter is the integer value that you want to save in the EEPROM memory.
 * @param startAddr The `startAddr` parameter in the `saveInt` function represents the starting address
 * in the EEPROM (Electrically Erasable Programmable Read-Only Memory) where the integer value will be
 * saved.
 * 
 * @return The `saveInt` function is returning the result of `EEPROM.commit()`, which is a boolean
 * value indicating whether the data was successfully written to the EEPROM memory.
 */
bool EEPROMLayoutManager::saveInt(int value, int startAddr) {
    EEPROM.put(startAddr, value);
    return EEPROM.commit();
}

/**
 * The function `loadInt` reads an integer value from EEPROM memory at a specified address and prints
 * the value to the Serial monitor.
 * 
 * @param startAddr The `startAddr` parameter in the `loadInt` function represents the starting address
 * in the EEPROM memory from which the integer value needs to be loaded.
 * 
 * @return The function `loadInt` is returning an integer value loaded from the EEPROM memory at the
 * specified `startAddr`.
 */
int EEPROMLayoutManager::loadInt(int startAddr) {
    int value;
    EEPROM.get(startAddr, value);
    return value;
}
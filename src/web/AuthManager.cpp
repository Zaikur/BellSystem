/*
Quinton Nelson
3/23/2024
This file handles the authentication of the device. It encrypts the password and generates a token for the device to use for authentication.
*/

#include "AuthManager.h"

/*********************PUBLIC*******************/

AuthManager::AuthManager() {}

/**
 * The `initialize` function in the `AuthManager` class initializes the authentication system by
 * loading necessary data from EEPROM, hashing and storing the default password if the device is not initialized,
 * and checking for the default password. If the default password is detected, a system message is added to the EEPROM.
 * 
 * @return The `initialize()` function in the `AuthManager` class is returning a boolean value `true`.
 */
bool AuthManager::initialize() {
    _salt = eepromManager.loadSalt();
    _initialized = eepromManager.loadInitialized();
    _encryptedPassword = eepromManager.loadPassword();

    if (!_initialized) {
        // If the device is not initialized, generate a random password and save it
        _salt = generateSalt(16);
        _encryptedPassword = hashPasswordWithSalt("admin", _salt);
        eepromManager.savePassword(_encryptedPassword);
        eepromManager.saveSalt(_salt);
        eepromManager.saveInitialized(true);
        _initialized = true;
    }

    if (checkPassword("admin")) {
        eepromManager.addSystemMessage("Default password detected. Change the default password as soon as possible.");
    }

    return true;
}

/**
 * The function `checkPassword` in the `AuthManager` class verifies if a given password matches the
 * stored encrypted password.
 * 
 * @param password The `password` parameter is a reference to a `String` object, which likely
 * represents the user's input password that needs to be checked for authentication.
 * 
 * @return The `checkPassword` function is returning a boolean value, which indicates whether the
 * hashed password with the salt matches the encrypted password stored in the `_encryptedPassword`
 * variable.
 */
bool AuthManager::checkPassword(const String &password) {
    return hashPasswordWithSalt(password, _salt) == _encryptedPassword;
}

/**
 * The function `updatePassword` in the `AuthManager` class updates the password by generating a new
 * salt, hashing the new password with the salt, saving the salt to EEPROM, and saving the encrypted
 * password to EEPROM.
 * 
 * @param newPassword The `newPassword` parameter is a `String` object that represents the new password
 * that the user wants to update to.
 * 
 * @return The `updatePassword` function returns a boolean value indicating whether the password update
 * operation was successful or not. It returns `true` if the password and salt were saved successfully
 * in the EEPROM, and `false` otherwise.
 */
bool AuthManager::updatePassword(const String &newPassword) {
    _salt = generateSalt(16);
    _encryptedPassword = hashPasswordWithSalt(newPassword, _salt);
    eepromManager.saveSalt(_salt);
    return eepromManager.savePassword(_encryptedPassword);
}

/**
 * The `generateToken` function in the `AuthManager` class creates a unique token by combining random
 * data, timestamp, and salt before hashing it.
 * 
 * @return The `generateToken` function returns the current token that is generated after combining
 * random data, current timestamp, and salt, and then hashing the combined data.
 */
String AuthManager::generateToken() {
    String tokenData = String(micros()) + String(random(0, INT32_MAX)) + _salt;
    _tokenTimestamp = millis();
    _currentToken = hashToken(tokenData);
    return _currentToken;
}

/**
 * The function `checkToken` in the `AuthManager` class checks if a given token matches the current
 * token and if the token is valid.
 * 
 * @param token The `token` parameter is a string that represents the token being checked for validity.
 * 
 * @return A boolean value is being returned.
 */
bool AuthManager::checkToken(const String &token) {
    return token == _currentToken && isTokenValid();
}


/****************PRIVATE******************/

/**
 * The function `generateSalt` in the `AuthManager` class generates a random string of a specified
 * length to be used as a salt for authentication purposes.
 * 
 * @param length The `length` parameter in the `generateSalt` function specifies the number of
 * characters that the generated salt string should have. This function generates a random salt string
 * of the specified length by selecting random characters from the ASCII range 33-126 and concatenating
 * them to form the salt string.
 * 
 * @return The `generateSalt` function returns a randomly generated string (salt) of the specified
 * length, where each character in the string is a random character within the ASCII range of 33-126.
 */
String AuthManager::generateSalt(size_t length) {
    String salt = "";
    for (size_t i = 0; i < length; i++) {
        // Generate a random character from ASCII range
        char randomChar = random(33, 126); // ASCII range: 33-126
        salt += randomChar;
    }
    return salt;
}

/**
 * The function `hashPasswordWithSalt` in C++ hashes a password with a salt using the SHA-256
 * algorithm.
 * 
 * @param password The `password` parameter is the user's password that needs to be hashed for secure
 * storage or transmission.
 * @param salt The `salt` parameter is a String that is used as an additional input to the password
 * hashing function. It is typically a random value that is combined with the password before hashing
 * to add complexity and prevent precomputed dictionary attacks.
 * 
 * @return The function `hashPasswordWithSalt` returns a hashed password generated by combining the
 * input password with a salt value, hashing the combined string using SHA-256 algorithm, and then
 * converting the resulting hash into a hexadecimal string before returning it.
 */
String AuthManager::hashPasswordWithSalt(const String &password, const String &salt) {
    br_sha256_context mc;
    br_sha256_init(&mc);
    String saltedPassword = salt + password; // Prepend
    br_sha256_update(&mc, saltedPassword.c_str(), saltedPassword.length());
    unsigned char hash[br_sha256_SIZE];
    br_sha256_out(&mc, hash);

    String result = "";
    for (size_t i = 0; i < sizeof(hash); i++) {
        if (hash[i] < 16) result += "0";
        result += String(hash[i], HEX);
    }
    return result;
}

/**
 * The `hashToken` function in C++ calculates the SHA-256 hash of the input data and returns it as a
 * hexadecimal string.
 * 
 * @param data The `hashToken` function takes a `String` reference named `data` as input. This `data`
 * parameter represents the data that needs to be hashed using the SHA-256 algorithm. The function
 * calculates the SHA-256 hash of the input data and returns the hashed result as a `String
 * 
 * @return The `hashToken` function returns a hashed token generated from the input data using the
 * SHA-256 algorithm. The hashed token is represented as a string of hexadecimal characters.
 */
String AuthManager::hashToken(const String &data) {
    br_sha256_context mc;
    br_sha256_init(&mc);
    br_sha256_update(&mc, data.c_str(), data.length());
    unsigned char hash[br_sha256_SIZE];
    br_sha256_out(&mc, hash);

    String result;
    for (size_t i = 0; i < sizeof(hash); i++) {
        char buf[3];
        sprintf(buf, "%02x", (unsigned int)hash[i]);
        result += String(buf);
    }
    return result;
}

/**
 * The function `isTokenValid` checks if the token is still valid based on the timestamp and validity
 * period.
 * 
 * @return The `isTokenValid()` function is returning a boolean value, which indicates whether the
 * token is still valid based on the comparison of the current time with the timestamp when the token
 * was issued.
 */
bool AuthManager::isTokenValid() {
    return (millis() - _tokenTimestamp) < _tokenValidityPeriod;
}

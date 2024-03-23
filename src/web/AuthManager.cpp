/*
Quinton Nelson
3/23/2024
This file handles the authentication of the device. It encrypts the password and generates a token for the device to use for authentication.
*/

#include "AuthManager.h"

/*********************PUBLIC*******************/

AuthManager::AuthManager() {
    // initial encrypted password (should be set properly during initialization)
    _salt = eepromManager.loadSalt();
    _initialized = eepromManager.loadInitialized();
    String password = eepromManager.loadPassword();

    if (!_initialized) {
        // If the device is not initialized, generate a random password and save it
        _salt = generateSalt(16);
        _encryptedPassword = hashPasswordWithSalt("admin", _salt);
        eepromManager.savePassword(_encryptedPassword);
        eepromManager.saveSalt(_salt);
        eepromManager.saveInitialized(false);
    }
}

bool AuthManager::checkPassword(const String &password) {
    return hashPasswordWithSalt(password, _salt) == _encryptedPassword;
}

bool AuthManager::updatePassword(const String &newPassword) {
    _salt = generateSalt(16);
    _encryptedPassword = hashPasswordWithSalt(newPassword, _salt);
    eepromManager.saveSalt(_salt);
    return eepromManager.savePassword(_encryptedPassword);
}

String AuthManager::generateToken() {
    // Generate a token using a combination of random data and the current timestamp plus salt before hashing
    // This guarantees that the data being hashed is unique for each token and is not predictable
    String tokenData = String(micros()) + String(random(0, INT32_MAX)) + _salt;
    _tokenTimestamp = millis();
    _currentToken = hashToken(tokenData);
    return _currentToken;
}

bool AuthManager::checkToken(const String &token) {
    return token == _currentToken && isTokenValid();
}


/****************PRIVATE******************/

String AuthManager::generateSalt(size_t length) {
    String salt = "";
    for (size_t i = 0; i < length; i++) {
        // Generate a random character from ASCII range
        char randomChar = random(33, 126); // ASCII range: 33-126
        salt += randomChar;
    }
    return salt;
}

// This method hashes the password with salt using SHA256
String AuthManager::hashPasswordWithSalt(const String &password, const String &salt) {
    br_sha256_context mc;
    br_sha256_init(&mc);
    String saltedPassword = salt + password; // Prepend or append
    br_sha256_update(&mc, saltedPassword.c_str(), saltedPassword.length());
    unsigned char hash[br_sha256_SIZE];
    br_sha256_out(&mc, hash);

    String result = "";
    for (unsigned int i = 0; i < sizeof(hash); i++) {
        if (hash[i] < 16) result += "0";
        result += String(hash[i], HEX);
    }
    return result;
}

// This method hashes the Token data using SHA256
String AuthManager::hashToken(const String &data) {
    br_sha256_context mc;
    br_sha256_init(&mc);
    br_sha256_update(&mc, data.c_str(), data.length());
    unsigned char hash[br_sha256_SIZE];
    br_sha256_out(&mc, hash);

    String result;
    for (unsigned int i = 0; i < sizeof(hash); i++) {
        char buf[3];
        sprintf(buf, "%02x", (unsigned int)hash[i]);
        result += String(buf);
    }
    return result;
}

bool AuthManager::isTokenValid() {
    return (millis() - _tokenTimestamp) < _tokenValidityPeriod;
}

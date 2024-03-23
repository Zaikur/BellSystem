/*
Quinton Nelson
3/23/2024
This file handles the authentication of the device. It encrypts the password and generates a token for the device to use for authentication.
*/


#ifndef AuthManager_h
#define AuthManager_h

#include <Arduino.h>
#include <BearSSLHelpers.h>

#include "board/EEPROMLayoutManager.h"

extern EEPROMLayoutManager eepromManager; // EEPROM manager object

class AuthManager {
  private:
    String _encryptedPassword; // Stores the encrypted password
    String _currentToken; // Stores the current token
    String _salt; // Stores the salt for password encryption
    bool _initialized; // Flag to check if the device has been initialized
    unsigned long _tokenTimestamp; // Timestamp when the token was generated
    const unsigned long _tokenValidityPeriod = 3600000; // Token validity period in milliseconds (e.g., 1 hour)

    String hashPasswordWithSalt(const String &password, const String &salt); // Hashes the password with the salt
    String generateSalt(size_t length); // Generates a random salt
    bool isTokenValid(); // Checks if the current token is still valid
    String hashToken(const String &data); // Hashes the data

  public:
    AuthManager();
    bool initialize();
    bool checkPassword(const String &password);
    bool updatePassword(const String &newPassword);
    String generateToken();
    bool checkToken(const String &token);
};

#endif

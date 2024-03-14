

#ifndef Server_h
#define Server_h

#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "RelayManager.h"
#include "WebPage.h"

class Server {
    public:
        Server();
        void handleRoot();
        void handleSettings();
        void handleSchedule();
        void handleNotFound();
        void handleError();
        void handleToggleRelay();
        void handleClientRequests();
    private:
        ESP8266WebServer server;
        void handleProtectedPage();
        bool checkAuth();
        const char* www_username = "admin";
        const char* www_password = "password";      //UPDATE THIS
};

#endif
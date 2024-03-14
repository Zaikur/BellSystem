/*
Quinton Nelson
3/13/2024
This file handles server setup and client requests
*/

#include "Server.h"

Server::Server() : server(80) { // Correct initialization of server member
    server.on("/", HTTP_GET, [this]() { this->handleRoot(); });
    server.on("/settings", HTTP_GET, [this]() { this->handleSettings(); });
    server.on("/schedule", HTTP_GET, [this]() { this->handleSchedule(); });
    server.on("/error", HTTP_GET, [this]() { this->handleError(); });
    server.on("/ToggleRelay", HTTP_GET, [this]() { this->handleToggleRelay(); });
    server.onNotFound([this]() { this->handleNotFound(); });
    server.begin();
}


//This method generates the home page for the server
void Server::handleRoot() {
    TimeManager timeClient;
    timeClient.updateTime();

    //Generate homepage using the WebPage class
    WebPage homePage("Home Page");
    String homePageHtml = homePage.generatePage();

    //Send the generated homepage to the client
    server.send(200, "text/html", homePageHtml);
}

void Server::handleProtectedPage() {
  if (!checkAuth()) {
    server.requestAuthentication();
    return;
  }
  // If authentication succeeds, serve the protected content
  server.send(200, "text/html", "<html><body><h1>Protected Content</h1></body></html>");
}

bool Server::checkAuth() {
    if (!server.hasHeader("Authorization")) {
        return false;
  }
    String authHeader = server.header("Authorization");
    if (authHeader.startsWith("Basic")) {
        authHeader = authHeader.substring(6); // Remove "Basic " prefix
        authHeader.trim(); // Trim whitespace
        String credentials = base64::encode(String(www_username) + ":" + String(www_password));
        if (authHeader.equals(credentials)) {
        return true;
        }
    }
    return false;
}

void Server::handleToggleRelay() {
  // Assuming the relay is connected to GPIO 5 (D1)
  Serial.println("Activate relay request received:");
  RelayManager relay(5);
  relay.activateRelay(2);
  server.send(200, "text/plain", "Relay toggle successful");
}

void Server::handleClientRequests() {
    server.handleClient();
}

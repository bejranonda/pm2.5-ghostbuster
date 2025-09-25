/*
 * PM2.5 Ghostbuster - WiFi Manager
 * Handles WiFi connection and management
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "config.h"

class WiFiManager {
private:
    unsigned long lastConnectAttempt = 0;
    bool isConnected = false;

public:
    WiFiManager();

    bool connect();
    bool reconnect();
    bool isWiFiConnected();
    void disconnect();
    String getLocalIP();
    String getMacAddress();
    int getRSSI();

    void handleConnection();  // Call this in loop()
};

#endif // WIFI_MANAGER_H
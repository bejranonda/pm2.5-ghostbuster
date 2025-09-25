/*
 * PM2.5 Ghostbuster - WiFi Manager Implementation
 */

#include "wifi_manager.h"

WiFiManager::WiFiManager() {
    WiFi.mode(WIFI_STA);
}

bool WiFiManager::connect() {
    #if ENABLE_SERIAL_DEBUG
        Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
    #endif

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        #if ENABLE_SERIAL_DEBUG
            Serial.print(".");
        #endif
    }

    isConnected = (WiFi.status() == WL_CONNECTED);

    if (isConnected) {
        #if ENABLE_SERIAL_DEBUG
            Serial.printf("\nWiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        #endif
    } else {
        #if ENABLE_SERIAL_DEBUG
            Serial.println("\nWiFi connection failed!");
        #endif
    }

    return isConnected;
}

bool WiFiManager::reconnect() {
    if (millis() - lastConnectAttempt >= WIFI_CONNECT_TIMEOUT) {
        lastConnectAttempt = millis();
        return connect();
    }
    return false;
}

bool WiFiManager::isWiFiConnected() {
    isConnected = (WiFi.status() == WL_CONNECTED);
    return isConnected;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    isConnected = false;
}

String WiFiManager::getLocalIP() {
    return WiFi.localIP().toString();
}

String WiFiManager::getMacAddress() {
    return WiFi.macAddress();
}

int WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

void WiFiManager::handleConnection() {
    if (!isWiFiConnected()) {
        #if ENABLE_SERIAL_DEBUG
            Serial.println("WiFi disconnected, attempting reconnection...");
        #endif
        reconnect();
    }
}
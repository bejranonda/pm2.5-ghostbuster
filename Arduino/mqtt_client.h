/*
 * PM2.5 Ghostbuster - MQTT Client
 * Handles MQTT connection and message publishing
 */

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>
#include <WiFiClient.h>
#include "config.h"

class MQTTClientManager {
private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    String deviceId;
    String clientId;

    // Topic strings
    String topicGPS;
    String topicAir;
    String topicSerial;
    String topicLWT;

    unsigned long lastConnectAttempt = 0;
    bool isConnected = false;

    void generateTopics();
    static void messageCallback(char* topic, byte* payload, unsigned int length);

public:
    MQTTClientManager();

    bool connect();
    bool reconnect();
    bool isMQTTConnected();
    void disconnect();
    void loop();  // Call this in main loop()

    bool publishGPS(double lat, double lon, double speed);
    bool publishAirQuality(double pm25, double lat, double lon, double speed = 0.0);
    bool publishSerial(const String& data);
    bool publish(const String& topic, const String& payload, bool retained = false);

    String getDeviceId() const;
    String getClientId() const;
};

#endif // MQTT_CLIENT_H
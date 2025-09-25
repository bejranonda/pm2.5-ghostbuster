/*
 * PM2.5 Ghostbuster - MQTT Client Implementation
 */

#include "mqtt_client.cpp"

MQTTClientManager::MQTTClientManager() : mqttClient(wifiClient) {
    // Generate unique device ID from ESP chip ID
    deviceId = String(ESP.getChipId(), HEX);
    clientId = String(MQTT_CLIENT_ID_PREFIX) + deviceId;

    generateTopics();

    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(messageCallback);
    mqttClient.setKeepAlive(MQTT_KEEPALIVE);
}

void MQTTClientManager::generateTopics() {
    String prefix = String(MQTT_TOPIC_PREFIX);

    topicGPS = prefix + "/" + deviceId + "/" + MQTT_TOPIC_GPS;
    topicAir = prefix + "/" + deviceId + "/" + MQTT_TOPIC_AIR;
    topicSerial = prefix + "/" + deviceId + "/" + MQTT_TOPIC_SERIAL;
    topicLWT = prefix + "/" + deviceId + "/" + MQTT_TOPIC_LWT;
}

bool MQTTClientManager::connect() {
    #if ENABLE_SERIAL_DEBUG
        Serial.printf("Connecting to MQTT broker: %s:%d\n", MQTT_BROKER, MQTT_PORT);
        Serial.printf("Client ID: %s\n", clientId.c_str());
    #endif

    // Connect with Last Will Testament
    String lwtMessage = "{\"_type\":\"lwt\",\"tst\":0,\"device_id\":\"" + deviceId + "\"}";

    if (mqttClient.connect(clientId.c_str(),
                          MQTT_USERNAME,
                          MQTT_PASSWORD,
                          topicLWT.c_str(),
                          1,
                          true,
                          lwtMessage.c_str())) {

        isConnected = true;

        #if ENABLE_SERIAL_DEBUG
            Serial.println("MQTT Connected!");
        #endif

        // Publish startup message
        String startupMsg = "{\"_type\":\"startup\",\"device_id\":\"" + deviceId + "\"}";
        mqttClient.publish(topicLWT.c_str(), startupMsg.c_str(), true);

        // Subscribe to serial commands (if needed)
        mqttClient.subscribe(topicSerial.c_str());

        return true;
    } else {
        isConnected = false;

        #if ENABLE_SERIAL_DEBUG
            Serial.printf("MQTT connection failed, error: %d\n", mqttClient.state());
        #endif

        return false;
    }
}

bool MQTTClientManager::reconnect() {
    if (millis() - lastConnectAttempt >= MQTT_RECONNECT_INTERVAL) {
        lastConnectAttempt = millis();
        return connect();
    }
    return false;
}

bool MQTTClientManager::isMQTTConnected() {
    isConnected = mqttClient.connected();
    return isConnected;
}

void MQTTClientManager::disconnect() {
    if (mqttClient.connected()) {
        mqttClient.disconnect();
    }
    isConnected = false;
}

void MQTTClientManager::loop() {
    if (!isMQTTConnected()) {
        #if ENABLE_SERIAL_DEBUG
            Serial.println("MQTT disconnected, attempting reconnection...");
        #endif
        reconnect();
    } else {
        mqttClient.loop();
    }
}

bool MQTTClientManager::publishGPS(double lat, double lon, double speed) {
    if (!isMQTTConnected()) return false;

    String payload = "{";
    payload += "\"lat\":" + String(lat, 6) + ",";
    payload += "\"lon\":" + String(lon, 6) + ",";
    payload += "\"speed\":" + String(speed, 2) + ",";
    payload += "\"tst\":" + String(millis() / 1000) + ",";
    payload += "\"device_id\":\"" + deviceId + "\"";
    payload += "}";

    return mqttClient.publish(topicGPS.c_str(), payload.c_str(), true);
}

bool MQTTClientManager::publishAirQuality(double pm25, double lat, double lon, double speed) {
    if (!isMQTTConnected()) return false;

    String payload = "{";
    payload += "\"pm25\":" + String(pm25, 2) + ",";
    payload += "\"lat\":" + String(lat, 6) + ",";
    payload += "\"lon\":" + String(lon, 6) + ",";
    payload += "\"speed\":" + String(speed, 2) + ",";
    payload += "\"tst\":" + String(millis() / 1000) + ",";
    payload += "\"device_id\":\"" + deviceId + "\"";
    payload += "}";

    bool result = mqttClient.publish(topicAir.c_str(), payload.c_str(), true);

    #if ENABLE_SERIAL_DEBUG
        if (result) {
            Serial.printf("Published air quality: PM2.5=%.2f at (%.6f,%.6f)\n", pm25, lat, lon);
        } else {
            Serial.println("Failed to publish air quality data");
        }
    #endif

    return result;
}

bool MQTTClientManager::publishSerial(const String& data) {
    if (!isMQTTConnected() || data.length() == 0) return false;

    return mqttClient.publish(topicSerial.c_str(), data.c_str(), true);
}

bool MQTTClientManager::publish(const String& topic, const String& payload, bool retained) {
    if (!isMQTTConnected()) return false;

    return mqttClient.publish(topic.c_str(), payload.c_str(), retained);
}

String MQTTClientManager::getDeviceId() const {
    return deviceId;
}

String MQTTClientManager::getClientId() const {
    return clientId;
}

void MQTTClientManager::messageCallback(char* topic, byte* payload, unsigned int length) {
    #if ENABLE_SERIAL_DEBUG
        Serial.printf("MQTT message received on [%s]: ", topic);
        for (unsigned int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    #endif
}
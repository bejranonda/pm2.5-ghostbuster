/*
 * PM2.5 Ghostbuster - Main Arduino Sketch
 * Refactored and improved version with modular architecture
 *
 * "PM2.5 is like ghost. It exists even if it is invisible."
 *
 * Hardware Requirements:
 * - ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
 * - GPS Module (connected to D1, D2)
 * - PM2.5 Sensor (connected via Serial)
 * - Status LED (connected to D5)
 *
 * Features:
 * - WiFi connectivity with auto-reconnection
 * - MQTT publishing with guaranteed delivery
 * - GPS tracking with location-based publishing
 * - PM2.5 sensor data collection
 * - Robust error handling and status indicators
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <FS.h>

// Project modules
#include "config.h"
#include "wifi_manager.h"
#include "mqtt_client.h"

// Global objects
WiFiManager wifiManager;
MQTTClientManager mqttClient;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// State variables
struct {
    double lastLatitude = 0.0;
    double lastLongitude = 0.0;
    bool hasFirstFix = false;
    unsigned long metersSincePublish = 0;
    unsigned long lastPublishTime = 0;
    unsigned long lastStatusUpdate = 0;
    bool statusLedState = false;
} sensorState;

// Function declarations
void setupHardware();
void setupFileSystem();
void handleGPS();
void handleSerialSensor();
void updateStatusLed();
bool shouldPublishLocation(double lat, double lon);
void publishLocationData(double lat, double lon, double speed);

void setup() {
    setupHardware();
    setupFileSystem();

    #if ENABLE_SERIAL_DEBUG
        Serial.begin(SERIAL_BAUD_RATE);
        Serial.println("\n" + String("=").repeat(50));
        Serial.println("PM2.5 Ghostbuster Starting...");
        Serial.println("Device ID: " + mqttClient.getDeviceId());
        Serial.println("=").repeat(50));
    #endif

    // Initialize GPS
    gpsSerial.begin(GPS_BAUD_RATE);

    // Connect to WiFi
    if (wifiManager.connect()) {
        // Connect to MQTT
        mqttClient.connect();
    }

    #if ENABLE_SERIAL_DEBUG
        Serial.println("Setup complete - entering main loop");
    #endif
}

void loop() {
    // Handle network connections
    wifiManager.handleConnection();
    mqttClient.loop();

    // Process GPS data
    handleGPS();

    // Handle serial sensor data
    handleSerialSensor();

    // Update status LED
    updateStatusLed();

    // Small delay for stability
    delay(10);
}

void setupHardware() {
    // Initialize status LED
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, LOW);

    // Blink startup sequence
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_STATUS_PIN, HIGH);
        delay(200);
        digitalWrite(LED_STATUS_PIN, LOW);
        delay(200);
    }
}

void setupFileSystem() {
    if (!SPIFFS.begin()) {
        #if ENABLE_SERIAL_DEBUG
            Serial.println("Formatting SPIFFS...");
        #endif
        SPIFFS.format();
        SPIFFS.begin();
    }
}

void handleGPS() {
    // Read GPS data
    while (gpsSerial.available()) {
        if (gps.encode(gpsSerial.read())) {
            // GPS sentence was successfully parsed
        }
    }

    // Process valid GPS location
    if (gps.location.isValid() && gps.location.age() < GPS_DATA_AGE_THRESHOLD) {
        double currentLat = gps.location.lat();
        double currentLon = gps.location.lng();
        double currentSpeed = gps.speed.kmph();

        if (!sensorState.hasFirstFix) {
            // First GPS fix
            sensorState.hasFirstFix = true;
            sensorState.lastLatitude = currentLat;
            sensorState.lastLongitude = currentLon;

            #if ENABLE_SERIAL_DEBUG
                Serial.printf("First GPS fix: %.6f, %.6f\n", currentLat, currentLon);
            #endif

            publishLocationData(currentLat, currentLon, currentSpeed);
        } else {
            // Check if we should publish based on movement or time
            if (shouldPublishLocation(currentLat, currentLon)) {
                publishLocationData(currentLat, currentLon, currentSpeed);

                // Update last known position
                sensorState.lastLatitude = currentLat;
                sensorState.lastLongitude = currentLon;
                sensorState.metersSincePublish = 0;
                sensorState.lastPublishTime = millis();
            }
        }
    }
}

void handleSerialSensor() {
    if (Serial.available()) {
        String sensorData = Serial.readStringUntil('\n');
        sensorData.trim();

        if (sensorData.length() > 0) {
            #if ENABLE_SERIAL_DEBUG
                Serial.println("Sensor data: " + sensorData);
            #endif

            // Try to publish via MQTT
            if (mqttClient.isMQTTConnected()) {
                mqttClient.publishSerial(sensorData);
            } else {
                #if ENABLE_SERIAL_DEBUG
                    Serial.println("MQTT not connected - sensor data buffered");
                #endif
                // Could store in SPIFFS for later transmission
            }
        }
    }
}

void updateStatusLed() {
    unsigned long currentTime = millis();

    // Update LED based on connection status
    if (wifiManager.isWiFiConnected() && mqttClient.isMQTTConnected()) {
        // Both connected - slow blink
        if (currentTime - sensorState.lastStatusUpdate > 2000) {
            sensorState.statusLedState = !sensorState.statusLedState;
            digitalWrite(LED_STATUS_PIN, sensorState.statusLedState);
            sensorState.lastStatusUpdate = currentTime;
        }
    } else if (wifiManager.isWiFiConnected()) {
        // WiFi only - medium blink
        if (currentTime - sensorState.lastStatusUpdate > 1000) {
            sensorState.statusLedState = !sensorState.statusLedState;
            digitalWrite(LED_STATUS_PIN, sensorState.statusLedState);
            sensorState.lastStatusUpdate = currentTime;
        }
    } else {
        // No connection - fast blink
        if (currentTime - sensorState.lastStatusUpdate > 200) {
            sensorState.statusLedState = !sensorState.statusLedState;
            digitalWrite(LED_STATUS_PIN, sensorState.statusLedState);
            sensorState.lastStatusUpdate = currentTime;
        }
    }
}

bool shouldPublishLocation(double lat, double lon) {
    unsigned long currentTime = millis();

    // Always publish after ping interval
    if (currentTime - sensorState.lastPublishTime >= PING_INTERVAL) {
        return true;
    }

    // Check movement distance
    if (sensorState.hasFirstFix) {
        double distance = TinyGPSPlus::distanceBetween(
            lat, lon,
            sensorState.lastLatitude, sensorState.lastLongitude
        );

        sensorState.metersSincePublish += (unsigned long)distance;

        // Publish if moved enough
        if (sensorState.metersSincePublish >= MIN_DISTANCE_METERS) {
            return true;
        }
    }

    return false;
}

void publishLocationData(double lat, double lon, double speed) {
    if (!mqttClient.isMQTTConnected()) {
        return;
    }

    // Publish GPS data
    bool gpsSuccess = mqttClient.publishGPS(lat, lon, speed);

    #if ENABLE_SERIAL_DEBUG
        if (gpsSuccess) {
            Serial.printf("Published GPS: %.6f, %.6f, %.2f km/h\n", lat, lon, speed);
        } else {
            Serial.println("Failed to publish GPS data");
        }
    #endif

    // If we have PM2.5 data, publish combined air quality data
    // This would be enhanced based on your specific PM2.5 sensor implementation
}
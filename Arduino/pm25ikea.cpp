#include <FS.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include "pico.h"

const char* ssid = WIFISSID;
const char* password = WIFIPASSWORD;
const char* mqtt_server = MQTTHOST;
const int mqtt_port = MQTTPORT;

#define BASETOPIC   "pm25"  // no trailing slash!
#define RECONNECTINTERVAL   60000
#define STORE       "/data/owntracks.log"
#define PINGINTERVAL        (60 * 60 * 1000)

#undef JSON
#ifdef JSON
# include <ArduinoJson.h>
#endif

#define LED_ALIVE   D5
#define RXpin       D2   // GPS RX
#define TXpin       D1   // GPS TX

TinyGPSPlus gps;
SoftwareSerial nss(RXpin, TXpin); // GPS serial

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long counter = 0;
unsigned long lastConnectAttempt = 0;
unsigned long pingInterval = 0;
bool is_online = false;

double last_lat, last_lon;
unsigned long trip = 0;
bool have_first_fix = false;
int mindist = 10;
unsigned long meters_since_pub;

static char deviceID[9];
static char pubtopic[128];
static char serialtopic[128];
static char lwttopic[128];

/// ---------- Helper functions ----------
void fs_prepare() {
  if (!SPIFFS.begin()) {
    Serial.println(F("Formatting FS..."));
    SPIFFS.format();
  }
  SPIFFS.begin();
}

bool setup_wifi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);
  return WiFi.status() == WL_CONNECTED;
}

bool check_online() {
  return (client.connected() && WiFi.status() == WL_CONNECTED);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT message on ["); Serial.print(topic); Serial.println("]");
}

bool MQTT_reconnect() {
  char clientID[64];
  snprintf(clientID, sizeof(clientID), "gps-node-%s", deviceID);

  if (client.connect(clientID, MQTTUSER, MQTTPASS, lwttopic, 1, true, "{\"_type\":\"lwt\",\"tst\":0}")) {
    Serial.println("Connected to MQTT");
    client.publish(lwttopic, "restart!");
    client.subscribe(serialtopic);
    return true;
  }
  return false;
}

void generateDeviceID() {
  char flashChipId[7];
  sprintf(flashChipId, "%06x", ESP.getFlashChipId());
  snprintf(deviceID, sizeof(deviceID), "%06x%s", ESP.getChipId(), flashChipId + strlen(flashChipId) - 2);
}

/// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);   // USB serial
  nss.begin(9600);        // GPS serial
  pinMode(LED_ALIVE, OUTPUT);

  fs_prepare();
  generateDeviceID();

  snprintf(pubtopic, sizeof(pubtopic), "%s/%s/gps", BASETOPIC, deviceID);
  snprintf(serialtopic, sizeof(serialtopic), "%s/%s/serial", BASETOPIC, deviceID);
  snprintf(lwttopic, sizeof(lwttopic), "%s/%s/LWT", BASETOPIC, deviceID);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Setup complete.");
}

/// ---------- LOOP ----------
void loop() {
  digitalWrite(LED_ALIVE, HIGH);
  delay(50);

  // keep WiFi/MQTT alive
  is_online = check_online();
  if (!is_online && millis() - lastConnectAttempt >= RECONNECTINTERVAL) {
    lastConnectAttempt = millis();
    if (setup_wifi()) is_online = MQTT_reconnect();
  }

  // --- Handle GPS ---
  while (nss.available()) gps.encode(nss.read());
  if (gps.location.isValid() && gps.location.age() < 2000) {
    if (!have_first_fix) {
      have_first_fix = true;
      last_lat = gps.location.lat();
      last_lon = gps.location.lng();
    } else {
      unsigned long meters = (unsigned long)TinyGPSPlus::distanceBetween(
        gps.location.lat(), gps.location.lng(), last_lat, last_lon
      );
      meters_since_pub += meters;
      if (meters_since_pub >= mindist || millis() - pingInterval >= PINGINTERVAL) {
        char payload[256];
        snprintf(payload, sizeof(payload),
          "{\"lat\":%.6f,\"lon\":%.6f,\"speed\":%.2f}",
          gps.location.lat(), gps.location.lng(), gps.speed.kmph()
        );
        if (is_online) client.publish(pubtopic, payload, true);
        meters_since_pub = 0;
        pingInterval = millis();
      }
      last_lat = gps.location.lat();
      last_lon = gps.location.lng();
    }
  }

  // --- Handle serial sensor (first project) ---
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      if (is_online) {
        client.publish(serialtopic, line.c_str(), true);
      } else {
        Serial.print("Buffered (offline): "); Serial.println(line);
      }
    }
  }

  digitalWrite(LED_ALIVE, LOW);
  delay(500);
}

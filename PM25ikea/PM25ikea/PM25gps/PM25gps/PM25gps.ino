#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

#include "Config.h"
#include "SerialCom.h"
#include "Types.h"

particleSensorState_t state;

uint8_t mqttRetryCounter = 0;

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient mqttClient;

WiFiManagerParameter custom_mqtt_server("server", "mqtt server", Config::mqtt_server, sizeof(Config::mqtt_server));
WiFiManagerParameter custom_mqtt_user("user", "MQTT username", Config::username, sizeof(Config::username));
WiFiManagerParameter custom_mqtt_pass("pass", "MQTT password", Config::password, sizeof(Config::password));

uint32_t lastMqttConnectionAttempt = 0;
const uint16_t mqttConnectionInterval = 60000; // 1 minute = 60 seconds = 60000 milliseconds

uint32_t statusPublishPreviousMillis = 0;
const uint16_t statusPublishInterval = 30000; // 30 seconds = 30000 milliseconds

char identifier[24];
#define FIRMWARE_PREFIX "pm25"
#define AVAILABILITY_ONLINE "online"
#define AVAILABILITY_OFFLINE "offline"
char MQTT_TOPIC_AVAILABILITY[128];
char MQTT_TOPIC_STATE[128];
char MQTT_TOPIC_COMMAND[128];

char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_PM25_SENSOR[128];

bool shouldSaveConfig = false;

//// GPS ////
#include <FS.h>
#include <TinyGPS++.h>
// #include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeLib.h>
// #include <SoftwareSerial.h>

#include "pico.h"

// const char* ssid = WIFISSID;
// const char* password = WIFIPASSWORD;
// const char* mqtt_server = MQTTHOST;
// const int mqtt_port = MQTTPORT;



// #define BASETOPIC	"pm25"	// no trailing slash!
#define RECONNECTINTERVAL	60000 // 300000			// ms (5m)
#define STORE		"/data/owntracks.log"
#define PINGINTERVAL		(60 * 60 * 1000)		// ms

/*
 * Define or undefine JSON to choose the * payload format to publish.
 * JSON will publish OwnTracks-type JSON payloads, but
 *      we have to limit number of elemements we add to
 *	the JSON in order to keep below PubSubClient's
 *	MAX_PACKET_SIZE.
 * CSV will publish in OwnTracks-type CSV (Greenwich)
 *	which is fully supported by the OwnTracks Recorder.
 */

#undef JSON


#ifdef JSON
# include <ArduinoJson.h>
#endif

#define LED_ALIVE	D5
#define RXpin	D2
#define TXpin	D1

TinyGPSPlus gps;
SoftwareSerial nss(RXpin, TXpin);	// RXpin, TXpin

static void smartdelay(unsigned long ms);
static void serialize(TinyGPSPlus &gps, char t);

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long counter = 0L;
unsigned long lastConnectAttempt = 0L;
unsigned long pingInterval = 0L;
bool is_online = false;		// true if WiFi and MQTT connected
double last_lat, last_lon;
unsigned long trip = 0L;
bool have_first_fix = false;
int mindist = 10;		// minimum distance in meters before 't'
unsigned long meters_since_pub;

static char deviceID[8 + 1];		// name of this device
static char pubtopic[128];
static char subtopic[128];
static char lwttopic[128];
/// END GPS ///

void saveConfigCallback() {
    shouldSaveConfig = true;
}

void setup() {
    // ESP.wdtEnable(WDTO_8S);
    // SerialCom::setup();
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Serial.begin");
    delay(100);
    // ESP.wdtFeed();
    nss.begin(9600);

    // Serial.println("-- SerialCom.begin");

    // Serial.begin(115200);
    // SerialCom::setup();

    // Serial.begin(9600);
    Serial.println("\n");
    // Serial.println("Hello from esp8266-vindriktning-particle-sensor");
    Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
    Serial.printf("Boot Version: %u\n", ESP.getBootVersion());
    Serial.printf("Boot Mode: %u\n", ESP.getBootMode());
    Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Reset reason: %s\n", ESP.getResetReason().c_str());

    delay(1000);

    snprintf(identifier, sizeof(identifier), "PM25ikea-%X", ESP.getChipId());
    snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", FIRMWARE_PREFIX, identifier);
    snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", FIRMWARE_PREFIX, identifier);
    snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", FIRMWARE_PREFIX, identifier);

    snprintf(MQTT_TOPIC_AUTOCONF_PM25_SENSOR, 127, "homeassistant/sensor/%s/%s_pm25/config", FIRMWARE_PREFIX, identifier);
    snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127, "homeassistant/sensor/%s/%s_wifi/config", FIRMWARE_PREFIX, identifier);

    WiFi.hostname(identifier);

    Config::load();

    setupWifi();
    setupOTA();
    mqttClient.setServer(Config::mqtt_server, 1883);
    mqttClient.setKeepAlive(10);
    mqttClient.setBufferSize(2048);
    mqttClient.setCallback(mqttCallback);

    Serial.printf("Hostname: %s\n", identifier);
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    Serial.println("-- Current GPIO Configuration --");
    Serial.printf("PIN_UART_RX: %d\n", SerialCom::PIN_UART_RX);

    mqttReconnect();

    //// GPS /////

    // nss.begin(9600);
    // Serial.println("-- SerialCom.begin");
    // SerialCom::setup();
    pinMode(LED_ALIVE, OUTPUT);


    fs_prepare();
    fs_show();

    generateDeviceID();

    Serial.print("deviceID=");
    Serial.println(deviceID);

    // snprintf(pubtopic, sizeof(pubtopic), "%s/%s", BASETOPIC, deviceID);
    // snprintf(subtopic, sizeof(subtopic), "%s/%s/CMND", BASETOPIC, deviceID);
    // snprintf(lwttopic, sizeof(subtopic), "%s/%s/LWT", BASETOPIC, deviceID);

    // client.setServer(mqtt_server, mqtt_port);
    // client.setCallback(callback);
}



void loop() {
    ArduinoOTA.handle();
    // SerialCom::handleUart(state);
    mqttClient.loop();

    // const uint32_t currentMillis = millis();
    // if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval) {
    //     statusPublishPreviousMillis = currentMillis;

    //     if (state.valid) {
    //         printf("Publish state\n");
    //         publishState();
    //     }
    // }

    // if (!mqttClient.connected() && currentMillis - lastMqttConnectionAttempt >= mqttConnectionInterval) {
    //     lastMqttConnectionAttempt = currentMillis;
    //     printf("Reconnect mqtt\n");
    //     mqttReconnect();
    // }

  /// GPS ///
	digitalWrite(LED_ALIVE, HIGH);
	smartdelay(50);

  // Check Available WiFi
  Serial.println(WiFi.status());
//  int n = WiFi.scanNetworks();
//  for (int i = 0; i < n; i++)
//  {
//    Serial.println(WiFi.SSID(i));
//  }
  // nss.begin(9600);
  // nss.listen();
  delay(100);
  int nssava = nss.available();
  mqttClient.publish(&MQTT_TOPIC_STATE[0], "Test", true);
  while (nss.available() > 0) {
		gps.encode(nss.read());
		// ESP.wdtFeed();
    mqttClient.publish(&MQTT_TOPIC_STATE[0], "OK", true);
    Serial.print("nss.available: "); Serial.println(nssava);
		yield();
	}


	is_online = check_online();

	unsigned long now = millis();
	// if (is_online == false && (now - lastConnectAttempt >= RECONNECTINTERVAL || !lastConnectAttempt)) {
	if (true) {
		lastConnectAttempt = now;

		bool have_wifi = setupWifi();

		if (have_wifi) {
			is_online = mqttReconnect();
			if (is_online) {
				/* Transition from OFF- to ON-line; unload */
				unload_store();

        if (state.valid) {
            printf("Publish state\n");
            publishState();
        }
        
			}
		}
	}

  Serial.print("gps.location.lng: ");  Serial.println(gps.location.lng());
  Serial.print("gps.satellites.value: ");  Serial.println(gps.satellites.value());
  Serial.print("gps.location.isValid: ");  Serial.println(gps.location.isValid());
  Serial.print("gps.location.age: ");  Serial.println(gps.location.age());
  
	// if (gps.location.isValid() && gps.location.age() < 2000) {
	if (gps.location.isValid()) {
		if (counter % 10 == 0) {
			gpstime_set(gps);
		}
		++counter;
		Serial.print("Age=");
		Serial.print(gps.location.age());
		Serial.print(" lat=");
		Serial.println(gps.location.lat());
		Serial.print(" lon=");
		Serial.println(gps.location.lng());

		Serial.print(" Y=");
		Serial.print(gps.date.year()); // Year (2000+) (u16)
		Serial.print(" M=");
		Serial.print(gps.date.month()); // Month (1-12) (u8)
		Serial.print(" D=");
		Serial.print(gps.date.day()); // Day (1-31) (u8)
		Serial.print(" T=");
		Serial.print(gps.time.value()); // Raw time in HHMMSSCC format (u32)
		Serial.print(" HH=");
		Serial.print(gps.time.hour()); // Hour (0-23) (u8)
		Serial.print(" MM=");
		Serial.print(gps.time.minute()); // Minute (0-59) (u8)
		Serial.print(" SS=");
		Serial.println(gps.time.second()); // Second (0-59) (u8)

		if (!have_first_fix) {
			serialize(gps, 'f');
			have_first_fix = true;
			last_lat = gps.location.lat();
			last_lon = gps.location.lng();
		} else {
			if (now - pingInterval >= PINGINTERVAL) {
				serialize(gps, 'p');
				pingInterval = millis();
			}

			unsigned long meters = (unsigned long)TinyGPSPlus::distanceBetween(
				gps.location.lat(), gps.location.lng(),
				last_lat, last_lon
				);

			meters_since_pub += meters;

			Serial.print("METERS=");
			Serial.print(meters);
			Serial.print(" SincePub=");
			Serial.println(meters_since_pub);

			if (meters_since_pub >= mindist) {
				serialize(gps, 'v');
				meters_since_pub = 0;
			}
			last_lat = gps.location.lat();
			last_lon = gps.location.lng();
		}

	} else {
		Serial.println("invalid location");
	}

	digitalWrite(LED_ALIVE, LOW);
	smartdelay(5000);
  
}

void setupOTA() {
    ArduinoOTA.onStart([]() { Serial.println("Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.setHostname(identifier);

    // This is less of a security measure and more a accidential flash prevention
    ArduinoOTA.setPassword(identifier);
    ArduinoOTA.begin();
}

bool setupWifi() {
    bool wifi_connected = WiFi.status() == WL_CONNECTED;

    if (wifi_connected)
      return true;
    delay(10);

    wifiManager.setDebugOutput(false);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_pass);

    WiFi.hostname(identifier);
    wifiManager.autoConnect(identifier);
    mqttClient.setClient(wifiClient);

    strcpy(Config::mqtt_server, custom_mqtt_server.getValue());
    strcpy(Config::username, custom_mqtt_user.getValue());
    strcpy(Config::password, custom_mqtt_pass.getValue());

    if (shouldSaveConfig) {
        Config::save();
    } else {
        // For some reason, the read values get overwritten in this function
        // To combat this, we just reload the config
        // This is most likely a logic error which could be fixed otherwise
        Config::load();
    }

    wifi_connected = WiFi.status() == WL_CONNECTED;

    Serial.println(wifi_connected ? "WiFi connected" : "WiFi NOT connected");

    return (wifi_connected);
}

void resetWifiSettingsAndReboot() {
    wifiManager.resetSettings();
    delay(3000);
    ESP.restart();
}

bool mqttReconnect() {
    for (uint8_t attempt = 0; attempt < 3; ++attempt) {
          Serial.print("*Connecting MQTT :");
          Serial.println(attempt);
        if (mqttClient.connect(identifier, Config::username, Config::password, MQTT_TOPIC_AVAILABILITY, 1, true, AVAILABILITY_OFFLINE)) {
            mqttClient.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
            publishAutoConfig();

            // Make sure to subscribe after polling the status so that we never execute commands with the default data
            mqttClient.subscribe(MQTT_TOPIC_COMMAND);
            delay(100);
            Serial.println("++MQTT connected");
            delay(100);
            return true; // Adapted
            // break;
        }
        delay(1000);
    }
  Serial.println("--MQTT NOT connect !!");
  return false; // Adapted
}

bool isMqttConnected() {
    return mqttClient.connected();
}

void publishState() {
    DynamicJsonDocument wifiJson(192);
    DynamicJsonDocument stateJson(604);
    char payload[256];

    wifiJson["ssid"] = WiFi.SSID();
    wifiJson["ip"] = WiFi.localIP().toString();
    wifiJson["rssi"] = WiFi.RSSI();

    stateJson["pm25"] = state.avgPM25;

    stateJson["wifi"] = wifiJson.as<JsonObject>();

    serializeJson(stateJson, payload);
    mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) { }

void publishAutoConfig() {
    char mqttPayload[2048];
    DynamicJsonDocument device(256);
    DynamicJsonDocument autoconfPayload(1024);
    StaticJsonDocument<64> identifiersDoc;
    JsonArray identifiers = identifiersDoc.to<JsonArray>();

    identifiers.add(identifier);

    device["identifiers"] = identifiers;
    device["manufacturer"] = "Ikea";
    device["model"] = "pm25ikea";
    device["name"] = identifier;
    device["sw_version"] = "2021.08.0";

    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = identifier + String(" WiFi");
    autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
    autoconfPayload["unique_id"] = identifier + String("_wifi");
    autoconfPayload["unit_of_measurement"] = "dBm";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["json_attributes_template"] = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": \"{{value_json.wifi.ip}}\"}";
    autoconfPayload["icon"] = "mdi:wifi";

    serializeJson(autoconfPayload, mqttPayload);
    mqttClient.publish(&MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[0], &mqttPayload[0], true);

    autoconfPayload.clear();

    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["name"] = identifier + String(" PM 2.5");
    autoconfPayload["unit_of_measurement"] = "μg/m³";
    autoconfPayload["value_template"] = "{{value_json.pm25}}";
    autoconfPayload["unique_id"] = identifier + String("_pm25");
    autoconfPayload["icon"] = "mdi:air-filter";

    serializeJson(autoconfPayload, mqttPayload);
    mqttClient.publish(&MQTT_TOPIC_AUTOCONF_PM25_SENSOR[0], &mqttPayload[0], true);

    autoconfPayload.clear();
}

//////////////
// GPS ///////
//////////////
// #include <FS.h>
// #include <TinyGPS++.h>
// #include <ESP8266WiFi.h>
// #include <PubSubClient.h>
// #include <TimeLib.h>
// #include <SoftwareSerial.h>

// #include "pico.h"

// const char* ssid = WIFISSID;
// const char* password = WIFIPASSWORD;
// const char* mqtt_server = MQTTHOST;
// const int mqtt_port = MQTTPORT;



// #define BASETOPIC	"pm25"	// no trailing slash!
// #define RECONNECTINTERVAL	60000 // 300000			// ms (5m)
// #define STORE		"/data/owntracks.log"
// #define PINGINTERVAL		(60 * 60 * 1000)		// ms

// /*
//  * Define or undefine JSON to choose the * payload format to publish.
//  * JSON will publish OwnTracks-type JSON payloads, but
//  *      we have to limit number of elemements we add to
//  *	the JSON in order to keep below PubSubClient's
//  *	MAX_PACKET_SIZE.
//  * CSV will publish in OwnTracks-type CSV (Greenwich)
//  *	which is fully supported by the OwnTracks Recorder.
//  */

// #undef JSON


// #ifdef JSON
// # include <ArduinoJson.h>
// #endif

// #define LED_ALIVE	D5
// #define RXpin	D2
// #define TXpin	D1

// TinyGPSPlus gps;
// SoftwareSerial nss(RXpin, TXpin);	// RXpin, TXpin

// static void smartdelay(unsigned long ms);
// static void serialize(TinyGPSPlus &gps, char t);

// WiFiClient espClient;
// PubSubClient client(espClient);
// unsigned long counter = 0L;
// unsigned long lastConnectAttempt = 0L;
// unsigned long pingInterval = 0L;
// bool is_online = false;		// true if WiFi and MQTT connected
// double last_lat, last_lon;
// unsigned long trip = 0L;
// bool have_first_fix = false;
// int mindist = 10;		// minimum distance in meters before 't'
// unsigned long meters_since_pub;

// static char deviceID[8 + 1];		// name of this device
// static char pubtopic[128];
// static char subtopic[128];
// static char lwttopic[128];

void fs_prepare()
{
	if (SPIFFS.begin() == false) {
		Serial.println(F("formatting filesystem..."));
		SPIFFS.format();
		Serial.println("done");
	}

	if (SPIFFS.begin() == false) {
		Serial.println(F("can't mount filesystem!"));
		return;
	}
}

void fs_show()
{
	Dir dir = SPIFFS.openDir("/data");

	Serial.println(F("-- directory ------------------"));
	while (dir.next()) {
		Serial.print(dir.fileName());
		Serial.print("  ");

		File f = dir.openFile("r");
		Serial.println(f.size());
		f.close();
	}
	Serial.println("-------------------------------");
	delay(500);
}


// bool setup_wifi()
// {
// 	bool wifi_connected = WiFi.status() == WL_CONNECTED;

// 	if (wifi_connected)
// 		return true;

// 	delay(10);

// 	Serial.println("Connecting to WiFi");

// 	WiFi.mode(WIFI_STA);
// 	WiFi.begin(ssid, password);

// 	smartdelay(1000);

// 	wifi_connected = WiFi.status() == WL_CONNECTED;

// 	Serial.println(wifi_connected ? "WiFi connected" : "WiFi NOT connected");

// 	return (wifi_connected);
// }

bool check_online()
{
	bool status = false;

	if (client.connected() && (WiFi.status() == WL_CONNECTED))
		status = true;
	return status;
}

void callback(char* topic, byte* payload, unsigned int length)
{

	// FIXME: handle cmd/ mindist
	// FIXME: handle cmd/list 	(list files)
	// FIXME: handle cmd/dump	(dump cache)

	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
}

// bool MQTT_reconnect()
// {
// 	char clientID[128];
// 	char *username = MQTTUSER;
// 	char *password = MQTTPASS;
// 	static char *willPayload = "{\"_type\":\"lwt\",\"tst\":0}";
// 	//bool willRetain = false;
//   bool willRetain = true;
// 	//char *willTopic = pubtopic;
//   char *willTopic = lwttopic;
// 	bool mqtt_connected = false;

// 	snprintf(clientID, sizeof(clientID), "micro-wifi-%s", deviceID);

// 	Serial.print("Attempting to connect to MQTT as ");
// 	Serial.println(clientID);

// 	client.disconnect();

//   if (client.connect(clientID, username, password, willTopic, MQTTQOS1, willRetain, willPayload)) {
// 		Serial.println("Connected to MQTT");
// 		// Once connected, publish an announcement...
// 		client.publish(lwttopic, "restart!");
// 		client.subscribe(subtopic);
// 		mqtt_connected = true;
// 	} else {
// 		Serial.println("NOT connected to MQTT");
// 	}

// 	return (mqtt_connected);
// }

/* swiped from Homie */
void generateDeviceID()
{
	char flashChipId[6 + 1];
	sprintf(flashChipId, "%06x", ESP.getFlashChipId());

	snprintf(deviceID, sizeof(deviceID), "%06x%s",
		ESP.getChipId(), flashChipId + strlen(flashChipId) - 2);
}

// void setup()
// {
// 	ESP.wdtEnable(WDTO_8S);
// 	//Serial.begin(115200);
// 	nss.begin(9600);
// 	pinMode(LED_ALIVE, OUTPUT);

// 	fs_prepare();
// 	fs_show();

// 	generateDeviceID();

// 	Serial.print("deviceID=");
// 	Serial.println(deviceID);

// 	snprintf(pubtopic, sizeof(pubtopic), "%s/%s", BASETOPIC, deviceID);
// 	snprintf(subtopic, sizeof(subtopic), "%s/%s/CMND", BASETOPIC, deviceID);
//   snprintf(lwttopic, sizeof(subtopic), "%s/%s/LWT", BASETOPIC, deviceID);

// 	client.setServer(mqtt_server, mqtt_port);
// 	client.setCallback(callback);
// }

// void loop()
// {
// 	digitalWrite(LED_ALIVE, HIGH);
// 	smartdelay(50);

//   // Check Available WiFi
//   Serial.println(WiFi.status());
// //  int n = WiFi.scanNetworks();
// //  for (int i = 0; i < n; i++)
// //  {
// //    Serial.println(WiFi.SSID(i));
// //  }

// 	while (nss.available() > 0) {
// 		gps.encode(nss.read());
// 		// ESP.wdtFeed();
// 		yield();
// 	}


// 	is_online = check_online();

// 	unsigned long now = millis();
// 	if (is_online == false && (now - lastConnectAttempt >= RECONNECTINTERVAL || !lastConnectAttempt)) {
// 		lastConnectAttempt = now;

// 		bool have_wifi = setup_wifi();

// 		if (have_wifi) {
// 			is_online = MQTT_reconnect();
// 			if (is_online) {
// 				/* Transition from OFF- to ON-line; unload */
// 				unload_store();
// 			}
// 		}
// 	}

// 	if (gps.location.isValid() && gps.location.age() < 2000) {
// 		if (counter % 10 == 0) {
// 			gpstime_set(gps);
// 		}
// 		++counter;
// 		Serial.print("Age=");
// 		Serial.print(gps.location.age());
// 		Serial.print(" lat=");
// 		Serial.println(gps.location.lat());
// 		Serial.print(" lon=");
// 		Serial.println(gps.location.lng());

// 		Serial.print(" Y=");
// 		Serial.print(gps.date.year()); // Year (2000+) (u16)
// 		Serial.print(" M=");
// 		Serial.print(gps.date.month()); // Month (1-12) (u8)
// 		Serial.print(" D=");
// 		Serial.print(gps.date.day()); // Day (1-31) (u8)
// 		Serial.print(" T=");
// 		Serial.print(gps.time.value()); // Raw time in HHMMSSCC format (u32)
// 		Serial.print(" HH=");
// 		Serial.print(gps.time.hour()); // Hour (0-23) (u8)
// 		Serial.print(" MM=");
// 		Serial.print(gps.time.minute()); // Minute (0-59) (u8)
// 		Serial.print(" SS=");
// 		Serial.println(gps.time.second()); // Second (0-59) (u8)

// 		if (!have_first_fix) {
// 			serialize(gps, 'f');
// 			have_first_fix = true;
// 			last_lat = gps.location.lat();
// 			last_lon = gps.location.lng();
// 		} else {
// 			if (now - pingInterval >= PINGINTERVAL) {
// 				serialize(gps, 'p');
// 				pingInterval = millis();
// 			}

// 			unsigned long meters = (unsigned long)TinyGPSPlus::distanceBetween(
// 				gps.location.lat(), gps.location.lng(),
// 				last_lat, last_lon
// 				);

// 			meters_since_pub += meters;

// 			Serial.print("METERS=");
// 			Serial.print(meters);
// 			Serial.print(" SincePub=");
// 			Serial.println(meters_since_pub);

// 			if (meters_since_pub >= mindist) {
// 				serialize(gps, 'v');
// 				meters_since_pub = 0;
// 			}
// 			last_lat = gps.location.lat();
// 			last_lon = gps.location.lng();
// 		}

// 	} else {
// 		Serial.println("invalid location");
// 	}

// 	digitalWrite(LED_ALIVE, LOW);
// 	smartdelay(5000);
// }

void store(char *payload)
{
	File f = SPIFFS.open(STORE, "a");

	if (!f) {
		Serial.println("Can't open file for append");
		return;
	}

	f.println(payload);
	Serial.print("store(): pos=");
	Serial.println(f.size());
	f.close();
}

void unload_store()
{
	File f = SPIFFS.open(STORE, "r");
	int rc, n = 0;

	if (!f) {
		Serial.println("Can't open file for unloading");
		return;
	}

	while (f.available()) {
		String line = f.readStringUntil('\n');

#if 1
		/* If WiFi / MQTT goes offline now we're screwed */
		if ((rc = client.publish(pubtopic, line.c_str(), true)) == false) {
			Serial.println("can't publish!");

			return; // keep file intact for next unload (it will have dup publishes!

		}
#endif
		Serial.print(n++);
		Serial.print(" ");
		Serial.print(f.position());
		Serial.print(" ");
		Serial.println(line);

		if (client.connected()) {
			client.loop();
			delay(20);
		} else {
			is_online = false;
			Serial.println("client no longer online in unload(); abort");
			return;
		}
	}
	f.close();

	/* If we reach this, we ought to be able to remove the store */

	SPIFFS.remove(STORE);
}

static void serialize(TinyGPSPlus &gps, char t)
{
#ifdef JSON
	DynamicJsonDocument doc(1024);
#endif
	char payload[1025];
	boolean rc;
	unsigned long meters = 0L;

	Serial.print("about to serialize t:");
	Serial.println(t);

	if (have_first_fix) {
		meters = (unsigned long)TinyGPSPlus::distanceBetween(
			gps.location.lat(), gps.location.lng(),
			last_lat, last_lon
			);
	}

	trip += meters;

#ifdef JSON
	char t_str[2] = { t, 0 };

	  doc["_type"] = "location";

	  doc["n"] = counter;
	  doc["lat"] = gps.location.lat();
	  doc["lon"] = gps.location.lng();
	  // doc["alt"] = int(gps.altitude.meters());
	  doc["vel"] = int(gps.speed.kmph());
    doc["pm25"] = int(state.avgPM25);
	  // doc["cog"] = int(gps.course.deg());
	  // doc["nsat"] = gps.satellites.value();
	  doc["tst"] = now();
	  doc["tid"] = deviceID + strlen(deviceID) - 2; // last 2 chars
	  doc["t"] = t;


	  serializeJson(doc, payload);
#else /* not JSON */
# define MILL 1000000.0

	unsigned int km_trip = (trip + 500) / 1000;
	snprintf(payload, sizeof(payload), "%s,%X,%c,%ld,%ld,%d,%d,%d,%u,%u",
		deviceID + strlen(deviceID) - 2,	// TID
		now(),					// tst in HEX
		t,					// t
		int(gps.location.lat() * MILL),		// lat
		int(gps.location.lng() * MILL),		// lon
		int(gps.course.deg() / 10),		// cog
		int(gps.speed.kmph()),			// vel
		int(gps.altitude.meters() / 10),	// alt
		meters,					// dist in meters
		km_trip);				// trip in km
#endif

	Serial.print("Status: ");
	Serial.print(is_online ? "Online" : "OFFline");
	Serial.print(" free heap=");
	Serial.println(ESP.getFreeHeap());

	if (is_online) {

		if ((rc = client.publish(pubtopic, payload, true)) == false) {
			store(payload);
		}
		Serial.print(" publish = ");
		Serial.println(rc);
		if (client.connected())
			client.loop();
	} else {
		store(payload);
	}

}

static void smartdelay(unsigned long ms)
{
	unsigned long start = millis();
	do
	{
		if (client.connected())
			client.loop();
		while (nss.available())
			gps.encode(nss.read());

		yield();
	} while (millis() - start < ms);
}

static void gpstime_set(TinyGPSPlus &gps)
{
	unsigned long age;
  	TimeElements t;
	time_t secs;

	t.Second	= gps.time.second();
	t.Minute	= gps.time.minute();
	t.Hour		= gps.time.hour();
	t.Day		= gps.date.day();
	t.Month		= gps.date.month();
	t.Year		= gps.date.year() - 1970;		// 2016

	secs = makeTime(t);
	setTime(secs);
}

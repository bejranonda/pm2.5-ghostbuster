import paho.mqtt.client as mqtt
import json
from influxdb import InfluxDBClient
from datetime import datetime, timedelta
import os
import geojson
import time

# MQTT settings
MQTT_BROKER_ADDRESS = "mqtt.thalay.eu"
MQTT_BROKER_PORT = 1883
MQTT_USERNAME = "pm25"
MQTT_PASSWORD = "FGsldku3ir"
MQTT_TOPIC = "pm25/+/air"  # the + will be replaced by the device ID

# InfluxDB settings
INFLUXDB_ADDRESS = "localhost"
INFLUXDB_PORT = 8086
INFLUXDB_DATABASE = "pm25gps"

# GeoJSON settings
GEOJSON_FILE = "../../var/www/html/gj/pm25gps.geojson"

# connect to InfluxDB
influxdb_client = InfluxDBClient(host=INFLUXDB_ADDRESS, port=INFLUXDB_PORT, database=INFLUXDB_DATABASE)

# define callback for MQTT messages
def on_message(client, userdata, message):
    try:
        topic_parts = message.topic.split("/")
        device_id = topic_parts[1]
        payload = json.loads(message.payload)
        pm25 = payload["pm25"]
        timestamp = datetime.utcfromtimestamp(payload["tst"])
        # timestamp = datetime.utcfromtimestamp(payload['tst']).strftime('%Y-%m-%dT%H:%M:%SZ')
        latitude = payload["lat"]
        longitude = payload["lon"]
        
        print("Data: ")
        print(device_id)
        
        # create JSON for InfluxDB
        data = [
            {
                "measurement": "air_quality",
                "tags": {
                    "device_id": device_id
                },
                "time": timestamp,
                "fields": {
                    "pm25": pm25,
                    "latitude": latitude,
                    "longitude": longitude
                }
            }
        ]
        
        print(data)
        
        # write data to InfluxDB
        influxdb_client.write_points(data)
    except Exception as e:
        print("Error on_message: ", e)
    
# create MQTT client
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(username=MQTT_USERNAME, password=MQTT_PASSWORD)
mqtt_client.on_message = on_message

# connect to MQTT broker and subscribe to topic
mqtt_client.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT)
# mqtt_client.subscribe(MQTT_TOPIC.replace("+", "#"))
mqtt_client.subscribe(MQTT_TOPIC)

# start loop to listen for MQTT messages
mqtt_client.loop_start()

# define function to create GeoJSON file
def create_geojson_file():
    # try:
        print("Start create_geojson_file")
        # get data from InfluxDB
        end_time = datetime.utcnow()
        start_time = end_time - timedelta(hours=24)
        start_time_str = start_time.strftime('%Y-%m-%dT%H:%M:%SZ')
        end_time_str = end_time.strftime('%Y-%m-%dT%H:%M:%SZ')
        query = 'SELECT * FROM "air_quality" WHERE time >= $start_time AND time <= $end_time'
        data = influxdb_client.query(query, bind_params={'start_time': start_time_str, 'end_time': end_time_str})
        print("reading database")
        print(data)
        # create GeoJSON features from data
        count = 0
        features = []
        for key in data.keys():
            print("key")
            for point in data[key]:
                feature = geojson.Feature(
                    geometry=geojson.Point((point["longitude"], point["latitude"])),
                    properties={
                        "device_id": point["device_id"],
                        "pm25": point["pm25"],
                        # "time": point["time"].isoformat()
                        "time": datetime.strptime(point["time"], '%Y-%m-%dT%H:%M:%SZ').isoformat()
                    }
                )
                features.append(feature)
                count += 1
                print(point["device_id"], " : ", point["pm25"])
        
        # create GeoJSON FeatureCollection and save to file
        feature_collection = geojson.FeatureCollection(features)
        with open(GEOJSON_FILE, "w") as f:
            geojson.dump(feature_collection, f)
        print(count)
        print(GEOJSON_FILE)
    # except Exception as e:
        # print("Error create_geojson_file: ", e)

    
# create GeoJSON file every 15 minutes
while True:
    # try:
        print("Creating JSON: ")
        create_geojson_file()
        time.sleep(900)  # Seconds / Standard 900 > 15 minutes
    # except Exception as e:
        # print("Error Creating JSON: ", e)

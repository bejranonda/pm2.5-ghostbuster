import paho.mqtt.client as mqtt
import json
from influxdb import InfluxDBClient
from datetime import datetime, timedelta
import os
import geojson
import time
import pytz

##### to start
# pm2 start GetDataFromPM25ikea.py
# pm2 start PM25ikeav2.py --interpreter python3

utc_tz = pytz.timezone('UTC')
bangkok_tz = pytz.timezone('Asia/Bangkok')

# MQTT settings
MQTT_BROKER_ADDRESS = "mqtt.thalay.eu"
MQTT_BROKER_PORT = 1883
MQTT_USERNAME = "pm25"
MQTT_PASSWORD = "FGsldku3ir"
MQTT_TOPIC = "pm25/#"  # the + will be replaced by the device ID

# define callback for MQTT messages
def on_message(client, userdata, message):
    try:
        print("Topic: ",message.topic)
        #print(message.topic)
        print("Payload: ",message.payload)
        #print(message.payload)
        payload = json.loads(message.payload)
        timestamp = datetime.utcfromtimestamp(payload['tst']).strftime('%Y-%m-%dT%H:%M:%SZ')
        print(timestamp)

    except Exception as e:
        print("Error on_message: ", e)

print("Connecting MQTT Broker")
# create MQTT client
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(username=MQTT_USERNAME, password=MQTT_PASSWORD)
mqtt_client.on_message = on_message

# connect to MQTT broker and subscribe to topic
mqtt_client.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT)
# mqtt_client.subscribe(MQTT_TOPIC.replace("+", "#"))
mqtt_client.subscribe(MQTT_TOPIC)

print("Starting Loop")
# start loop to listen for MQTT messages
mqtt_client.loop_forever()
import pymongo
import paho.mqtt.client as mqtt
from datetime import datetime


# MongoDB configuration
mongo_client = pymongo.MongoClient("mongodb://localhost:27017/")
db = mongo_client["shelter"]
collection = db["shelter"]


# MQTT configuration

# be sure to change this, as the ip address is ephemeral
mqtt_broker_address = '34.171.169.110'

# iot topic
mqtt_topic_shelter = 'shelter'



def on_message(client, userdata, message):
    payload = message.payload.decode('utf-8')
    print(f'Received message: {payload}')
    # Convert MQTT timestamp to datetime
    timestamp = datetime.utcnow() # Use current UTC time
    datetime_obj = timestamp.strftime("%Y-%m-%dT%H:%M:%S.%fZ")
    # Process the payload and insert into MongoDB with proper timestamp
    document = {
    'timestamp': datetime_obj,
    'data': payload
    }
    collection.insert_one(document)
    print('Data ingested into MongoDB')

client = mqtt.Client()
client.on_message = on_message

# Connect to MQTT broker
client.connect(mqtt_broker_address, 1883, 60)
# Subscribe to MQTT topic
client.subscribe(mqtt_topic_shelter)


# Start the MQTT loop
client.loop_forever()
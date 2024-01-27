#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Arduino_JSON.h>


//DHT
#define DHTPIN 40
#define DHTTYPE DHT11

//Rain
#define RAINAO A7
#define RAINDO 4

//Servo
#define SERVO_PIN 42

//MQTT server conf
// Not needed for V-ONE, they have their own.
const char *MQTT_SERVER= "34.171.169.110";
const int MQTT_PORT = 1883;

// Wifi conf 
// change this
const char *WIFI_SSID = "cslab";
const char *WIFI_PASSWORD = "aksesg31";




///////////////////////////
// Class instantiation
///////////////////////////

DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;
WiFiClient espClient;
PubSubClient client(espClient);

//Variables
int DO = 0;
int tmp = 0;
char *RainingOrNot = "Not Raining";
char *OpenOrClose = "Open";

// MQTT topics
// const char *MQTT_TOPIC_TEMPERATURE = "temperature";
// const char *MQTT_TOPIC_HUMIDITY = "humidity";
// const char *MQTT_TOPIC_RAIN = "rain";
// const char *MQTT_TOPIC_SERVO = "servo";
const char *MQTT_TOPIC_SHELTER = "shelter";


void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}


void setup()
{
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  servoMotor.attach(SERVO_PIN);
  
  // Setup rain digital pimode
    pinMode(RAINDO, INPUT);
}


void reconnect()
{
  while (!client.connected())
{
    Serial.println("Attempting MQTT connection...");
  if (client.connect("ESP32Client"))
  {
    Serial.println("Connected to MQTT server");
  }
  else
  {
  Serial.print("Failed, rc=");
  Serial.print(client.state());
  Serial.println(" Retrying in 5 seconds...");
  delay(5000);
}
}
}

void loop()
{

///////////////////////////////////////
  delay(1000);
  if (!client.connected())
  {
    reconnect();
  } 
  client.loop();
  delay(5000);

///////////////////////////////////////

  //read the moisture content in %
  float humidity = dht.readHumidity();
  
  //read the temp in celcius
  float temperature = dht.readTemperature();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed reception");
    return;
    //Returns an error if the ESP32 does not receive any measurements
  }



// Print to serial////////////
  Serial.print("Humidity: "); Serial.print(humidity);
  Serial.print("%  Temperature: "); Serial.print(temperature); Serial.print("°C, ");
//////////////////////////////

  tmp=digitalRead(RAINDO);
  DO=tmp;
  if(DO == 0){
    Serial.println("D=0");
    servoMotor.write(0);
    RainingOrNot = "Raining";
    OpenOrClose = "Close";
  }
  if(DO == 1){
    Serial.print("D=");
    Serial.println(DO);
    servoMotor.write(180);
    RainingOrNot = "Not Raining";
    OpenOrClose = "Open";
  }
///////////////////////////



// Construct payload to send to GCP//
delay(500);
  JSONVar payload;

  payload["RainingOrNot"] = RainingOrNot;
  payload["OpenOrClose"] = OpenOrClose;
  payload["temperature"] = temperature;
  payload["humidity"] = humidity;


  Serial.println(payload);

// // rain payload
//     client.publish(MQTT_TOPIC_RAIN, RainingOrNot);

// // servo payload
//     client.publish(MQTT_TOPIC_SERVO, OpenOrClose);

// // temperature payload
//     char temperaturePayload[10];
//     sprintf(temperaturePayload, "%.2f", temperature);
//     client.publish(MQTT_TOPIC_TEMPERATURE, temperaturePayload);


// //  humidity payload
//     char humidityPayload[10];
//     sprintf(humidityPayload, "%.2f", humidity);
//     client.publish(MQTT_TOPIC_HUMIDITY, humidityPayload);

// Send combined JSON data in one topic

client.publish(MQTT_TOPIC_SHELTER, JSON.stringify(payload).c_str());

    
    
//////////////////////////////////////

}
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"
#include <TimeLib.h>
#include <SPI.h> 

#define FIREBASE_HOST "caminfo-884b1-default-rtdb.firebaseio.com"
#define WIFI_SSID "desmoph"
#define WIFI_PASSWORD "desmo@16"

#define DHTPIN 2                                            // Digital pin connected to DHT11
#define DHTTYPE DHT11                                        // Initialize dht type as DHT 11
DHT dht(DHTPIN, DHTTYPE);

int Gas_analog = A0;

void setup()
{
  pinMode(Gas_analog, INPUT);
  Serial.begin(115200);
  dht.begin();                                                  //reads dht sensor data
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());                               //prints local IP address
  Firebase.begin(FIREBASE_HOST);                 // connect to the firebase

}

void loop()
{
  int gassensorAnalog = analogRead(Gas_analog);
  Serial.print("Gas Sensor: ");
  Serial.print(gassensorAnalog);
  float h = dht.readHumidity();                                 // Read Humidity
  float t = dht.readTemperature();                              // Read temperature

  if (gassensorAnalog > 300)                                     // Checking sensor working
  {
    Serial.println("Gas");
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  String fireHumid = String(h) + String("%");                   //Humidity integer to string conversion

  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C ");
  String fireTemp = String(t) + String("°C");                  //Temperature integer to string conversion

  
  //Firebase.pushInt("/Gas", gassensorAnalog);
  //Firebase.pushString("/Humidity", fireHumid);            //setup path to send Humidity readings
  //Firebase.pushString("/Temperature", fireTemp);         //setup path to send Temperature readings
  
  StaticJsonBuffer<50> jsonBuffer1;
  JsonObject& timeStampObject = jsonBuffer1.createObject();
  timeStampObject[".sv"] = "timestamp";
  
  StaticJsonBuffer<256> jsonBuffer;
  JsonArray& arrayTemp = jsonBuffer.createArray();
  arrayTemp.add(timeStampObject);
  arrayTemp.add(fireTemp);
  Firebase.push("/Temperature", arrayTemp);
  
  JsonArray& arrayGas = jsonBuffer.createArray();
  arrayGas.add(timeStampObject);
  arrayGas.add(gassensorAnalog);
  Firebase.push("/Gas", arrayGas);
  
  JsonArray& arrayHumidity = jsonBuffer.createArray();
  arrayHumidity.add(timeStampObject);
  arrayHumidity.add(fireHumid);
  Firebase.push("/Humidity", arrayHumidity);
}

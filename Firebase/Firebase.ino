#include <WiFiManager.h>        
#include "FirebaseESP8266.h"    
#include "DHT.h"               
#define FIREBASE_HOST "sampler-analogread-default-rtdb.firebaseio.com"
#define FIREBASE_KEY "NiAyeXFIRaJAJ9hyDrmPmJOylsiZkqBWtISZnX2X"

#define ESP_AP "WEMOS D1"
#define password  "analogread"
WiFiServer server(80);
#define DHTPIN 2                                               // what pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

//Define FirebaseESP8266 data object
FirebaseData firebaseData;

void setup()
{
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);
  Firebase.reconnectWiFi(true);

  dht.begin();
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //fetches ssid and password from EEPROM and tries to connect
  //if it does not connect, it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(ESP_AP, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
  }
  Serial.println(" \n\n\n\n\n");
  Serial.println("------------------");
  Serial.println("| WiFi connected |");
  Serial.println("------------------");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" \n\n\n\n\n");
  server.begin();

}
unsigned long timer = millis();
void loop()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  if (millis() - timer >= 500) {
    timer += 500;
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.print(" *F\t\n");
    sensorUpdate(h, t);
  }

}
void sensorUpdate(float h, float t) {
  if (Firebase.setFloat(firebaseData, "/SensorIOT/Humidity", h)) {
    Serial.println(" ____________________________________");
    Serial.println("|   Successfully uploaded :Humidity  |");
  }
  else {
    Serial.println(" ERROR!!!  REASON: " + firebaseData.errorReason());
    Serial.println(" -------------------------------------\n\n\n\n");
  }
  if (Firebase.setFloat(firebaseData, "/SensorIOT/Temperature", t)) {
    Serial.println("| Successfully uploaded :Temperature |");
    Serial.println(" -------------------------------------\n\n\n\n");
  }
  else {
    Serial.println(" ERROR!!!  REASON: " + firebaseData.errorReason());
    Serial.println(" -------------------------------------\n\n\n\n");
  }
}

#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"
#include "DHT.h"

const char* ssid = "IoT";
const char* password = "ASCIIASCII";
#define DHTPIN 2
DHT dht(DHTPIN, DHT22);

const int ledPin = 0;  // manually configure LED pin


WebThingAdapter* adapter;

const char* deviceTypesT[] = {"TemperatureSensor", nullptr};
const char* deviceTypesH[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice deviceT("DHT-T", "DHT22 01-t", deviceTypesT);
ThingDevice deviceH("DHT-H", "DHT22 01-h", deviceTypesH);
ThingProperty hProperty("Humidity", "DHT-h", NUMBER, "LevelProperty");
ThingProperty tProperty("temperature", "", NUMBER, "TemperatureProperty");
// ThingDevice led("led", "WiFi Relay 10", ledTypes);
// ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");

float lastT = 0;

float lastH = 0;
unsigned long lastTime;

void setup(void) {
  dht.begin();
  lastTime = millis();
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("w25", WiFi.localIP());
  tProperty.unit = "celsius";
  tProperty.readOnly = true;
  hProperty.unit = "percent";
  hProperty.readOnly = true;
  deviceH.addProperty(&hProperty);
  deviceT.addProperty(&tProperty);
  adapter->addDevice(&deviceT);
  adapter->addDevice(&deviceH);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");

}

void loop(void) {
  unsigned long time = millis();
  if (time < lastTime || time - lastTime > 3000) {
    float t = dht.readTemperature();                  // Запрос на считывание температуры
    float h = dht.readHumidity();

    if (lastH != h || lastT != t) {

      ThingPropertyValue tValue;
      tValue.number = t;
      tProperty.setValue(tValue);
      ThingPropertyValue hValue;
      hValue.number = h;
      hProperty.setValue(hValue);

      lastH = h;
      lastT = t;
      lastTime = time;
    }
    adapter->update();
  }



}

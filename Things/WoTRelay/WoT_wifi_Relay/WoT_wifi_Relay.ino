#include <Arduino.h>
#include "WebThingAdapter.h"
//https://habr.com/ru/post/390593/
const char* ssid = "SCP Foundation 2G";
const char* password = "Earthn2160";


const int ledPin = 12;  // manually configure LED pin


WebThingAdapter* adapter;

const char* ledTypes[] = {"OnOffSwitch", "Light", nullptr};
ThingDevice led("led", "WiFi Relay SN1", ledTypes);
ThingProperty ledOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void setup(void){
  pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, LOW);
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
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   // digitalWrite(ledPin, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  //digitalWrite(ledPin, HIGH); // active low led

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("w25", WiFi.localIP());

  led.addProperty(&ledOn);
  adapter->addDevice(&led);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(led.id);
}

void loop(void){
  adapter->update();
  bool on = ledOn.getValue().boolean;
  if (on != lastOn) {
    Serial.print(led.id);
    Serial.print(": ");
    Serial.println(on);
      digitalWrite(ledPin, on ? LOW : HIGH); // active low led

  }
  lastOn = on;
}

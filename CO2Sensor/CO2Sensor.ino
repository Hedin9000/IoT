#include <MHZ19_uart.h>

/*
   The MySensors Arduino library handles the wireless radio link and protocol
   between your home built sensors/actuators and HA controller of choice.
   The sensors forms a self healing radio network with optional repeaters. Each
   repeater and gateway builds a routing tables in EEPROM which keeps track of the
   network topology allowing messages to be routed to nodes.

   Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
   Copyright (C) 2013-2019 Sensnology AB
   Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   DESCRIPTION

    MH-Z14 CO2 sensor

    Wiring:
     Pad 1, Pad 5: Vin (Voltage input 4.5V-6V)
     Pad 2, Pad 3, Pad 12: GND
     Pad 6: PWM output ==> pin 6

 	From: http://davidegironi.blogspot.fr/2014/01/co2-meter-using-ndir-infrared-mh-z14.html
  	  MH-Z14 has a PWM output, with a sensitivity range of 0ppm to 2000ppm CO2, an accuracy of ±200ppm.
  	  The cycle is 1004ms±5%, given the duty cicle Th (pulse high), Tl is 1004-Th, we can convert it to CO2 value using the formula:
 	  CO2ppm = 2000 * (Th - 2ms) /(Th + Tl - 4ms)
  	From: http://airqualityegg.wikispaces.com/Sensor+Tests
 	  - response time is less than 30 s
     - 3 minute warm up time
 	datasheet: http://www.futurlec.com/Datasheet/Sensor/MH-Z14.pdf
   Contributor: epierre
*/

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>
#include <SPI.h>
#include <DHT.h>
#define CHILD_ID_AIQ 0
#define DHT_DATA_PIN 3
#define SENSOR_TEMP_OFFSET 0
static const uint8_t FORCE_UPDATE_N_READS = 10;
uint32_t SLEEP_TIME = 30 * 1000; // Sleep time between reads (in milliseconds)
#define CHILD_ID_HUM 1
#define CHILD_ID_TEMP 2
float valAIQ = 0.0;
float lastAIQ = 0.0;
MHZ19_uart mhz19;
MyMessage msg(CHILD_ID_AIQ, V_LEVEL);
MyMessage msg2(CHILD_ID_AIQ, V_UNIT_PREFIX);
#define MHZ_RX 6
#define MHZ_TX 5
float lastTemp;
float lastHum;
uint8_t nNoUpdatesTemp;
uint8_t nNoUpdatesHum;
bool metric = true;

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

#define DHTTYPE DHT22 
#define DHTPIN 3  
DHT dht(DHTPIN, DHTTYPE);


void setup()
{
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(true);
//  while ( mhz19.isWarming() ) {
//    Serial.print("MH-Z19 now warming up...  status:"); Serial.println(mhz19.getStatus());
//    delay(1000);
//  }

 dht.begin();


}

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("AIQ Sensor CO2 MH-Z19B", "3.0");

  // Register all sensors to gateway (they will be created as child devices)
  present(CHILD_ID_AIQ, S_AIR_QUALITY);
   present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  send(msg2.set("ppm"));
}

void loop()
{

  //uint32_t duration = pulseIn(AIQ_SENSOR_ANALOG_PIN, HIGH);
  long co2ppm = mhz19.getPPM();
  Serial.println(co2ppm);
  send(msg2.set((int32_t)ceil(co2ppm)));
  //-----------------------------------------------------------------

  //--------------------------------------------------------------

  // Get temperature from DHT library
  float temperature = dht.readTemperature(); 
  if (isnan(temperature)) {
    Serial.println("Failed reading temperature from DHT!");
  } else if (true) {
    // Only send temperature if it changed since the last measurement or if we didn't send an update for n times
    lastTemp = temperature;

    // apply the offset before converting to something different than Celsius degrees
    temperature += SENSOR_TEMP_OFFSET;

   
    // Reset no updates counter
    nNoUpdatesTemp = 0;
    send(msgTemp.set(temperature, 1));

    #ifdef MY_DEBUG
    Serial.print("T: ");
    Serial.println(temperature);
    #endif
  } else {
    // Increase no update counter if the temperature stayed the same
    nNoUpdatesTemp++;
  }

  // Get humidity from DHT library
  float humidity = dht.readHumidity(); 
  if (isnan(humidity)) {
    Serial.println("Failed reading humidity from DHT");
  } else if (true) {
    // Only send humidity if it changed since the last measurement or if we didn't send an update for n times
    lastHum = humidity;
    // Reset no updates counter
    nNoUpdatesHum = 0;
    send(msgHum.set(humidity, 1));

    #ifdef MY_DEBUG
    Serial.print("H: ");
    Serial.println(humidity);
    #endif
  } else {
    // Increase no update counter if the humidity stayed the same
    nNoUpdatesHum++;
  }





  sleep(SLEEP_TIME); //sleep for: sleepTime
}

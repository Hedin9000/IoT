/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2019 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * This is an example that demonstrates how to report the battery level for a sensor
 * Instructions for measuring battery capacity on A0 are available here:
 * http://www.mysensors.org/build/battery
 *
 */

#include <Keypad.h> // includes the keypad library
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
 {'0','1','2','3'},
 {'4','5','6','7'},
 {'8','9','A','B'},
 {'C','D','E','F'}
};

byte rowPins[ROWS] = {4,5,6,7}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {A1,A2,A3,A4}; // connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int CHILD_ID = 0;
// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>
//#define MY_NODE_ID 3
int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point


MyMessage on(CHILD_ID, V_SCENE_ON);
MyMessage off(CHILD_ID, V_SCENE_OFF);

void setup()
{

}

void presentation()
{
	// Send the sketch version information to the gateway and Controller

   present(CHILD_ID, S_SCENE_CONTROLLER);

	sendSketchInfo("RemoteKeyboard", "0.1");
}

void loop()
{
char key = keypad.getKey(); 
  if (key != NO_KEY){        
      send(on.set(key));
  }
}

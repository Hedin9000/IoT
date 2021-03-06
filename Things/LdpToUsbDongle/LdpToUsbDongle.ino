/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
      
}

void loop() {
  if (mySwitch.available()) {
  long data =  mySwitch.getReceivedValue() ;
    Serial.println(data);

    mySwitch.resetAvailable();
  }
}

#include "Arduino.h"
#include "PCF8574.h"

// Set the i2c HEX address
PCF8574 pcf8574(0x3F);

void setup(){
  Serial.begin(115200);
  delay(1000);
  // Set the pinModes
  pcf8574.pinMode(T3, OUTPUT);
  pcf8574.begin();
}

void loop(){
  pcf8574.digitalWrite(T3, HIGH);

  delay(1000);
  pcf8574.digitalWrite(T3, LOW);

  delay(5000);
}

#include "PCF8574.h"
PCF8574(uint8_t 0x3F,uint8_t 15,uint8_t 14);
void setup() {
Serial.begin(115200);
pcf8574.pinMode(P3, OUTPUT);

}

void loop() {
 Serial.println("HIGH");
PCF8574.digitalWrite(P1, HIGH);

delay(1000);

Serial.println("LOW");
PCF8574.digitalWrite(P3, LOW);
delay(5000);
}

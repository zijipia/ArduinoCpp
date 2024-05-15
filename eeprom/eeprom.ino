#include "EEPROM.h"
void setup() {
Serial.begin(115200);
EEPROM.begin(12);
for (int i = 0; i < 512; i++) {
 EEPROM.write(i, 0);
 }
EEPROM.commit();
delay(500);
Serial.println("ok");
}

void loop() {
  // put your main code here, to run repeatedly:

}

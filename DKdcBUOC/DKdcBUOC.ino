#include <Stepper.h>
Stepper myStepper(800, 3,4);
int chieu = 0; 
void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), Check, FALLING);
}
void loop() {
  int motorSpeed = map(analogRead(A0), 0, 1023, 0, 4*1000);
  if (motorSpeed > 0) {
  myStepper.step( chieu ? 800/100 : -800/100);
  myStepper.setSpeed(motorSpeed);
  }
}
void Check(){
  chieu = !chieu;
  delay(500);
}

#define GREEN_LED 9
#define RED_LED 10
#define BLUE_LED 11

int brightness, rBright, gBright, bBright;
int fadeSpeed = 10;
void setup() {
   pinMode(GREEN_LED, OUTPUT);
   pinMode(RED_LED, OUTPUT);
   pinMode(BLUE_LED, OUTPUT);
}

void loop() {
brightness = 255;
rBright = 0;
gBright = 0;
bBright = 0;
  TurnOn();
  delay(5000);
  TurnOff();
  delay(5000);
}

void TurnOn() { 
   for (int i = 0; i < 256; i++) {
       analogWrite(RED_LED, rBright);
       rBright += 1;
       delay(fadeSpeed);
   }

   for (int i = 0; i < 256; i++) {
       analogWrite(BLUE_LED, bBright);
       bBright += 1;
       delay(fadeSpeed);
   } 

   for (int i = 0; i < 256; i++) {
       analogWrite(GREEN_LED, gBright);
       gBright += 1;
       delay(fadeSpeed);
   } 
}

void TurnOff() {
   for (int i = 0; i < 256; i++) {
       analogWrite(GREEN_LED, brightness);
       analogWrite(RED_LED, brightness);
       analogWrite(BLUE_LED, brightness);
       brightness -= 1;
       delay(fadeSpeed);
   }
}
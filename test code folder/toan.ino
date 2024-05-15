#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4
int a = 0;
int b = 0;
long time ;
long time2;
float tocdo;
void DEM(){
a++;    }
void setup() {
lcd.init();
lcd.backlight();
pinMode(2, INPUT_PULLUP);
pinMode(A0, INPUT);
attachInterrupt(0, DEM , FALLING);  }
void loop() {
if( ( (long)millis()-time) > 1000){
b = a;
a = 0;
time = millis();  }   
if( ( (long)millis()-time2) > 200){
lcd.clear();
time2 = millis();  }
tocdo = map(analogRead(A0) , 0, 1023, 0 , 255);
analogWrite(6, tocdo);
lcd.setCursor(0,0);
lcd.print("RPM:");
lcd.print(b*60);
lcd.setCursor(0,1);
lcd.print("N:");
lcd.print(b);
lcd.setCursor(8,1);
lcd.print("PWM:");
lcd.print((int)((tocdo/255)*100));
lcd.print("%");
}
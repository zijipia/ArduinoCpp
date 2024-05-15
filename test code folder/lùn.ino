unsigned long xung;
float nuoc = 0;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4
void setup(){
lcd.init();                    
lcd.backlight();
pinMode(2, INPUT);
attachInterrupt(0, demxung, RISING);
}
void demxung(){
xung ++;
}
void loop(){
lcd.setCursor(0 , 0); lcd.print("Liter: "); lcd.print( (float)xung/5880 );
lcd.setCursor(0 , 1); lcd.print("Cm^3: "); lcd.print(nuoc);
nuoc=((float)xung/5880)*1000;
}
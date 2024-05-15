#include <Wire.h>
#include "RTClib.h"

#include <EEPROM.h>

RTC_DS1307 rtc;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4

char daysOfTheWeek[7][12] = {"CN", "HAI", "BA", "TU", "NAM", "SAU", "BAY"};

const int len = 2;
const int xuong = 3;
const int ok = 4;
const int k1 = 5;
const int k2 = 6;
const int leds = 7;
const int  sensor = A0;
byte vitri = 0; byte a = 0;
byte gio = 0; byte phut = 0;
byte kt1 = 0,kt2 = 0 ;
byte on = 0 , off = 0 ;byte giatri;
void setup () {
  Serial.begin(9600);
  pinMode(len, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(k1, OUTPUT);
  pinMode(k2, OUTPUT);
  pinMode(leds, OUTPUT);
  digitalWrite(k1, 0);digitalWrite(k2, 0);digitalWrite(leds, 0);
  lcd.init();                    
  lcd.backlight();

  rtc.begin();
  rtc.isrunning();
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//tự động cập nhật từ thời gian máy tính
//rtc.adjust(DateTime(2023, 4, 23, 00, 28, 0)); // cày giờ theo ý muốn
}

void loop () {
  thoigian();
  checktime();
  checksensor();
  giatri = map(analogRead(sensor),0,1023,0,10);
  Serial.println(giatri);
  if(!digitalRead(ok)){ delay(200);
  if(digitalRead(ok)){ lcd.clear(); menu(); } }
}

void checksensor(){
  if(giatri >= EEPROM.read(11)  ) { digitalWrite(leds, 1); }
  if (giatri <= EEPROM.read(10)){ digitalWrite(leds, 0); }
}

void checktime(){
  DateTime now = rtc.now();
  EEPROM.write(8, kt1);EEPROM.write(9,kt2);

  if(EEPROM.read(8) == 1){
  digitalWrite(k1, 1);}
  if(EEPROM.read(9) == 1){
  digitalWrite(k2, 1);}

  if( now.hour() == EEPROM.read(0) && now.minute() == EEPROM.read(1) ){
  digitalWrite(k1, 1); kt1 = 1;}
  if(now.hour() == EEPROM.read(2) && now.minute() == EEPROM.read(3)){
  digitalWrite(k1, 0); kt1 = 0;}

  if( now.hour() == EEPROM.read(4) && now.minute() == EEPROM.read(5)){
  digitalWrite(k2, 1); kt2 = 1; }
  if(now.hour() == EEPROM.read(6) && now.minute() == EEPROM.read(7)){
  digitalWrite(k2, 0); kt2 = 0;}
}

void menu(){
  while(1){
  if(!digitalRead(ok)){ delay(200);
  if(digitalRead(ok)){ lcd.clear(); a++; }  }
  if(a < 2){
  lcd.setCursor(1, 0);
  lcd.print("SET TIME 1");
  lcd.setCursor(1, 1);
  lcd.print("SET TIME 2");
  lcd.setCursor(0, a);
  lcd.print(">"); }
  else{
  lcd.setCursor(0, a-2);
  lcd.print(">");
  lcd.setCursor(1, 0);
  lcd.print("SET SENSOR");
  if(!digitalRead(len) || !digitalRead(xuong)){ delay(100);
  if((digitalRead(len) || digitalRead(xuong)) && a == 2){
  lcd.clear();delay(200);setsensor(); lcd.clear(); }  }  }

  if(a == 3) { a = 0; break; vitri = 0;}

  if(!digitalRead(len) || !digitalRead(xuong)){ delay(100);
  if((digitalRead(len) || digitalRead(xuong)) && a == 0){
  lcd.clear();delay(200);time1(); lcd.clear(); }  }

  if(!digitalRead(len) || !digitalRead(xuong)){ delay(100);
  if((digitalRead(len) || digitalRead(xuong)) && a == 1){
  lcd.clear();delay(200);time2(); lcd.clear(); } } } }

void setsensor(){
  off = EEPROM.read(10);on = EEPROM.read(11);
  while(digitalRead(ok)){
  if(giatri == 10 ) { delay(100); lcd.clear();}
  EEPROM.write(10, off); delay(5); EEPROM.write(11, on);delay(5);
  checksensor();
  giatri = map(analogRead(sensor),0,1023,0,10);
  lcd.setCursor(0,0);
  lcd.print("OFF- SENSOR -ON");
  lcd.setCursor(0,1);
  lcd.print(off);
  lcd.setCursor(7,1);
  lcd.print(giatri);
  lcd.setCursor(14,1);
  lcd.print(on);
  if(!digitalRead(xuong)){ delay(200); off ++; if(off > 10) { off = 0; lcd.clear();}}
  if(!digitalRead(len)){ delay(200); on ++;if(on > 10) { on = 0; lcd.clear();}  }  }
}

void time1(){
  while(digitalRead(ok)){
  gio = EEPROM.read(0); phut = EEPROM.read(1);
  lcd.setCursor(3,0);
  lcd.print("ON TIME 1");
  lcd.setCursor(0,1);
  lcd.print(gio);
  lcd.print(":GIO");
  lcd.setCursor(9,1);
  lcd.print(phut);
  lcd.print(":PHUT");
  if(!digitalRead(xuong)){ delay(200);gio ++; if(gio > 23) { gio = 0; lcd.clear();  } }
  if(!digitalRead(len)){ delay(200);phut ++; if(phut > 59) { phut = 0; lcd.clear(); }  }
  EEPROM.write(0, gio); delay(5); EEPROM.write(1, phut);delay(5); }
  gio = 0; phut = 0;
  while(!digitalRead(ok)) {  }
  lcd.clear();
  while(digitalRead(ok)){
  gio = EEPROM.read(2); phut = EEPROM.read(3);
  lcd.setCursor(3,0);
  lcd.print("OFF TIME 1");
  lcd.setCursor(0,1);
  lcd.print(gio);
  lcd.print(":GIO");
  lcd.setCursor(9,1);
  lcd.print(phut);
  lcd.print(":PHUT");
  if(!digitalRead(xuong)){ delay(200);gio ++; if(gio > 23) { gio = 0; lcd.clear();  } }
  if(!digitalRead(len)){ delay(200);phut ++; if(phut > 59) { phut = 0; lcd.clear(); }  }
  EEPROM.write(2, gio); delay(5); EEPROM.write(3, phut); delay(5); }
  gio = 0; phut = 0;
  while(!digitalRead(ok)) {  }    }

void time2(){
  while(digitalRead(ok)){
  gio = EEPROM.read(4); phut = EEPROM.read(5);
  lcd.setCursor(3,0);
  lcd.print("ON TIME 2");
  lcd.setCursor(0,1);
  lcd.print(gio);
  lcd.print(":GIO");
  lcd.setCursor(9,1);
  lcd.print(phut);
  lcd.print(":PHUT");
  if(!digitalRead(xuong)){ delay(200);gio ++; if(gio > 23) { gio = 0; lcd.clear();  } }
  if(!digitalRead(len)){ delay(200);phut ++; if(phut > 59) { phut = 0; lcd.clear(); }  }
  EEPROM.write(4, gio); delay(5); EEPROM.write(5, phut);delay(5); }
  gio = 0; phut = 0;
  while(!digitalRead(ok)) {  }
  lcd.clear();
  while(digitalRead(ok)){
  gio = EEPROM.read(6); phut = EEPROM.read(7);
  lcd.setCursor(3,0);
  lcd.print("OFF TIME 2");
  lcd.setCursor(0,1);
  lcd.print(gio);
  lcd.print(":GIO");
  lcd.setCursor(9,1);
  lcd.print(phut);
  lcd.print(":PHUT");
  if(!digitalRead(xuong)){ delay(200);gio ++; if(gio > 23) { gio = 0; lcd.clear();  } }
  if(!digitalRead(len)){ delay(200);phut ++; if(phut > 59) { phut = 0; lcd.clear(); }  }
  EEPROM.write(6, gio); delay(5); EEPROM.write(7, phut); delay(5); }
  gio = 0; phut = 0;
  while(!digitalRead(ok)) {  }    }

void thoigian(){
    DateTime now = rtc.now();
    lcd.setCursor(0, 1);
    lcd.print("TIME");
    lcd.print("  ");
    lcd.print(now.hour());
    lcd.print(':');
    lcd.print(now.minute());
    lcd.print(':');
    lcd.print(now.second());
    lcd.print("  ");

    lcd.setCursor(0, 0);
    lcd.print("DATE");
    lcd.print(" ");
    //lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.print(" ");
    lcd.print(now.day());
    lcd.print('/');
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.year());
    lcd.print("  ");
}

#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
RTC_DS1307 rtc;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

char daysOfTheWeek[7][12] = {"CN", "HAI", "BA", "TU", "NAM", "SAU", "BAY"};

byte customChar[] = {
  B00000,
  B00110,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

long time = 0;

int hienthi; 

const int len = 3;
 
const int xuong = 4;

int ngayds,thangds,namds = 2023,giods,phutds,giayds;

void setup () {
dht.begin();
lcd.init();                    
lcd.backlight(); 
lcd.createChar(0, customChar);
rtc.begin();
rtc.isrunning();
pinMode(len, INPUT_PULLUP);pinMode(xuong, INPUT_PULLUP);
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//tự động cập nhật từ thời gian máy tính
//rtc.adjust(DateTime(2023, 4, 23, 00, 28, 0)); // cày giờ theo ý muốn
}

void loop () {
  if( (millis() - time) > 5000){
  lcd.clear();hienthi ++;
  time = millis();}
  else if(hienthi%2 == 0){
  NHIETDO_DOAM();}
  else { TIME_DS1307(); }
  if(!digitalRead(len)&&!digitalRead(xuong)){ delay(200); MENU(); delay(200); lcd.clear();}
}
void MENU(){
  lcd.clear();
  byte giatriout = 0, S = 100;
  while(!digitalRead(len) && !digitalRead(xuong)) { }
  while(1){
  htds();
  rtc.adjust(DateTime(namds, thangds, ngayds, giods, phutds, giayds)); // cày giờ theo ý muốn
  delay(100);   while(!digitalRead(len) && !digitalRead(xuong)) { }

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  if(ngayds < 10){
  lcd.setCursor(5, 0); lcd.blink(); delay(50);lcd.noBlink();}
  else{
  lcd.setCursor(6, 0); lcd.blink(); delay(50);lcd.noBlink(); }
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); ngayds++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); ngayds--; } if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); } } while(!digitalRead(len) && !digitalRead(xuong) ) { } delay(200);

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  if(thangds < 10){
  lcd.setCursor(8, 0); lcd.blink(); delay(50);lcd.noBlink(); }
  else{
  lcd.setCursor(9, 0); lcd.blink(); delay(50);lcd.noBlink(); }
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); thangds++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); thangds--;} if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); }} while(!digitalRead(len) && !digitalRead(xuong)) { } delay(200);

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  lcd.setCursor(14, 0); lcd.blink(); delay(50);lcd.noBlink();
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); namds++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); namds--;}if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); } } while(!digitalRead(len) && !digitalRead(xuong)) { } delay(200);

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  if(giods < 10){
  lcd.setCursor(5, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  else{
  lcd.setCursor(6, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); giods++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); giods--;} if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); } } while(!digitalRead(len) && !digitalRead(xuong)) { } delay(200);

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  if(phutds < 10){
  lcd.setCursor(8, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  else{
  lcd.setCursor(9, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); phutds++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); phutds--;} if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); } } while(!digitalRead(len) && !digitalRead(xuong)) { } delay(200);

  while(digitalRead(len) || digitalRead(xuong)) {
  htds();dkds();
  if(giayds < 10){
  lcd.setCursor(11, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  else{
  lcd.setCursor(12, 1); lcd.blink(); delay(50);lcd.noBlink(); }
  if(!digitalRead(len) && digitalRead(xuong) ) { delay(S); giayds++; } else if(digitalRead(len) && !digitalRead(xuong) ){ delay(S); giayds--;}  giatriout = 1; if(!digitalRead(len) || !digitalRead(xuong) ) {lcd.clear(); }}

  rtc.adjust(DateTime(namds, thangds, ngayds, giods, phutds, giayds));

  break;delay(100);   }
}

void htds(){
  lcd.setCursor(0, 0);
  lcd.print("DATE");

  lcd.setCursor(5, 0);
  lcd.print(ngayds); 

  lcd.setCursor(7, 0);
  lcd.print("/");

  lcd.setCursor(8, 0);
  lcd.print(thangds);

  lcd.setCursor(10, 0);
  lcd.print("/");

  lcd.setCursor(11, 0);
  lcd.print(namds);

  lcd.setCursor(0, 1);
  lcd.print("TIME");

  lcd.setCursor(5, 1);
  lcd.print(giods);

  lcd.setCursor(7, 1);
  lcd.print("/");
  lcd.setCursor(8, 1);
  lcd.print(phutds);

  lcd.setCursor(10, 1);
  lcd.print("/");
  lcd.setCursor(11, 1);
  lcd.print(giayds);

}

void dkds(){

  if(ngayds > 31) { ngayds = 0; lcd.clear();}
  else if(ngayds < 0) { ngayds = 31;}

  if(thangds > 12) { thangds = 0;lcd.clear();}
  else if(thangds < 0) { thangds = 12;}

  if(namds > 2050) { namds = 2023;lcd.clear();}
  else if(namds < 2023) { namds = 2023;;}

  if(giods > 23) { giods = 0;lcd.clear();}
  else if(giods < 0) { giods = 23;}

  if(phutds > 59) { phutds = 0;lcd.clear();}
  else if(phutds < 0) { phutds = 59;}

  if(giayds > 59) { giayds = 0;lcd.clear();}
  else if(giayds < 0) { giayds = 59;}
}

void TIME_DS1307(){
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

void NHIETDO_DOAM(){
  float t = dht.readTemperature();
  lcd.setCursor(0,0);lcd.print("NHIET DO:");lcd.print(t);lcd.write(byte(0));lcd.print("C");

  float h = dht.readHumidity(); lcd.setCursor(0,1);lcd.print("DO AM: ");lcd.print(h);lcd.print(" %");
}

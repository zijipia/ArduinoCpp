#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"


#define SDA 15
#define SCL 14
#define COLUMNS 16
#define ROWS    2

LiquidCrystal_I2C lcd(0x27, COLUMNS, ROWS);  

const char* ssid     = "iPhone";
const char* password = "Huytho@1769";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  lcd.begin();
  lcd.clear();           
  lcd.backlight();                      
  
  lcd.setCursor(2,0);
  lcd.print("Dang Ket Noi");  
  lcd.setCursor(6,1);
  lcd.print("WIFI");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
    lcd.clear();   
    lcd.setCursor(0,0);
    lcd.print("Da Ket Noi WIFI!");
    Serial.println("CONNECTED to WIFI");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
    lcd.clear(); 
    lcd.setCursor(2,0);
    lcd.print(&timeinfo, "%a/%d/%m/%y");
    lcd.setCursor(4,1);
    lcd.print(&timeinfo, "%H:%M:%S");
  Serial.println(&timeinfo, "%A,%d %m  %y %H:%M:%S");
}

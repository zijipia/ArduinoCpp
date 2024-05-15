#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Keypad_I2C.h>
#include <WiFi.h>
#include "time.h"

const char* ssid     = "Nha Tro Tien Nghi P19";
const char* password = "123456789";

#define I2Ckey  0x3F
#define I2Clcd  0x27
#define Password_Lenght 5
#define SDA 15
#define SCL 14
LiquidCrystal_I2C lcd(I2Clcd, 16, 2);

unsigned long timee;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {0, 1, 2, 4};
byte colPins[COLS] = {5, 6, 7};
Keypad_I2C kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2Ckey, PCF8574 );

char Data[Password_Lenght]; 
char Data2[Password_Lenght];
char Master[Password_Lenght];
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char key;
byte key_state = 0;
char last_press_key;
byte mode = 4;

int change_password_allow_time = 10000;
long time_old = 0;
bool just_allowed_pass =0;

char FistTimePassword[] = {'1','2','3','4'};

void setup(){
  Wire.begin(SDA, SCL);
  kpd.begin( makeKeymap(keys) );
  Serial.begin(115200);
  Check_EEPROM();
  Serial.println(Master);
  lcd.begin();
  lcd.clear();           
  lcd.backlight(); 
  pinMode(4, OUTPUT);

  digitalWrite(4, LOW);
   lcd.setCursor(2,0);
  lcd.print("Dang Ket Noi");  
  lcd.setCursor(6,1);
  lcd.print("WIFI");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
    timee = millis();
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
  key = kpd.getKey(); 
  key_state = kpd.getState();

  if(key){
    if( mode == 4 ){
      lcd.clear();
      mode = 0;
      }
    last_press_key = key;
    Serial.println(key);
  }

if( mode == 3){
    if(last_press_key == '#' && key_state == 2){
      mode = 1;
    }
    if(last_press_key == '*' && key_state == 2){
      mode = 4;
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Da Khoa");
    digitalWrite(4, LOW); 
    delay(2000);          
    }        
}

  if(mode == 0){
    lcd.setCursor(0,0);
    lcd.print("Nhap Mat Khau:");  
  }else if(mode == 1){
    lcd.setCursor(0,0);
    lcd.print("Nhap MKhau Moi");    
  }else if(mode == 2){
    lcd.setCursor(0,0);
    lcd.print("Nhap Lai MK");      
  }else if(mode == 3){
    lcd.setCursor(3,0);
    lcd.print("Da Mo Khoa");         
  }else if(mode == 4){
if ( (unsigned long) (millis() - timee) > 1000)
    {
    printLocalTime();
    }
  }


  if (key && key != '#' && mode != 3) 
  {
    collectKey();
  }

  if(data_count == Password_Lenght-1) 
  {
    if(mode == 0){
      lcd.clear();
      Serial.println(Master);
      if(!strcmp(Data, Master)) {
        lcd.setCursor(2, 0);
        lcd.print("Xin Chao");
        just_allowed_pass = 1;
        time_old = millis();
        digitalWrite(4, HIGH);
        
        delay(2000);
        mode = 3;
      }else{
        lcd.setCursor(2, 0);
        lcd.print("Mat Khau");
        lcd.setCursor(4, 1);
        lcd.print("Khong Dung!");
        delay(2000);
        lcd.clear();
        mode = 4;
      }
      delay(1000);// 
      lcd.clear();
      clearData(); 
      
     // digitalWrite(4, LOW); 
    }else if( mode == 1){
      lcd.clear();
      mode = 2;
      for(int i = 0; i < Password_Lenght; i = i+1){
      Data2[i] = Data[i];
     }
      clearData(); 
    }else if(mode == 2){
      if(!strcmp(Data, Data2)){
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("MK moi la:");  
        lcd.setCursor(4, 1);   
        lcd.print(Data);  
        delay(2000);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Saving...");
        for(int i =0; i <= 100; i =  i+1){
          lcd.setCursor(4, 1);
          lcd.print(i);
          lcd.setCursor(7, 1);
          lcd.print("%");
          delay(10);
        }
        EEPROM.put(0, Data);
        EEPROM.get(0, Master);
        EEPROM.commit();
        delay(500);
      }else{
        lcd.clear();
        lcd.setCursor(4, 0); 
        lcd.print("Mat Khau");  
        lcd.setCursor(3, 1);   
        lcd.print("Khong Dung!");  
        delay(2000);        
      }
      mode = 3;
      clearData();
      lcd.clear();
    }
  }
}

void collectKey(){
  Data[data_count] = key; 
  lcd.setCursor(4+data_count,1); 
  lcd.print("*"); 
  data_count++;   
}

void clearData()
{
  while(data_count !=0)
  {  
    Data[data_count--] = 0; 
  }
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
    timee = millis();
}

void Check_EEPROM(){
  EEPROM.begin(12);
  EEPROM.get(0, Master);
  Serial.println(Master);
  if(Master[0] == 0 && Master[1] == 0 && Master[2] == 0 && Master[3] == 0){
    Serial.println("No EEPROM PASSWORD FOUND");
    EEPROM.put(0, FistTimePassword);
    EEPROM.get(0, Master);
    EEPROM.commit();
  }
}

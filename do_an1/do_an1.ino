//them thu vien can thiet
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Keypad_I2C.h>
#include "time.h"
//wifi
const char* ssid     = "Nha Tro Tien Nghi P19";
const char* password = "123456789";
//bot telegram
String BOTtoken = "6029268732:AAF0LAu1RqWU3OW9agZRm3ZmJhNlJqRzEmU";
String CHAT_ID = "6179690055";

//def
#define I2Ckey  0x3F
#define I2Clcd  0x27
#define Password_Lenght 5
#define SDA 15
#define SCL 14
//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define FLASH_LED_PIN 2
bool flashState = LOW;

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
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
//setup

void setup(){

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Wire.begin(SDA, SCL);
  kpd.begin( makeKeymap(keys) );
  Serial.begin(115200);
  Check_EEPROM();
  Serial.println(Master);
  lcd.begin();
  lcd.clear();           
  lcd.backlight(); 
  //config camera
  configInitCamera();
  //khoa (lock)
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  //Ket noi Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.setCursor(2,0);
  lcd.print("Dang Ket Noi");  
  lcd.setCursor(6,1);
  lcd.print("WIFI");

  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Add root certificate for api.telegram.org

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
    timee = millis();
    lcd.clear();   
    lcd.setCursor(0,0);
    lcd.print("Da Ket Noi WIFI!");
    Serial.println("CONNECTED to WIFI");
    Serial.println();
    Serial.print("ESP32-CAM IP Address: ");
    Serial.println(WiFi.localIP()); 
    //time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

  // Set LED Flash as output
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);
}
//loop
void loop(){
   if (sendPhoto) {
    Serial.println("Preparing photo");
    sendPhotoTelegram(); 
    sendPhoto = false; 
    }
    if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
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
        sendPhoto = true;
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
//config cam
void configInitCamera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  // Drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the ESP32-CAM \n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggles flash LED \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Change flash LED state");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
    }
    if (text == "/unlock") {
      sendPhoto = true;
        lcd.setCursor(4, 0);
        lcd.print("Xin chào");
        just_allowed_pass = 1;
        time_old = millis();
        digitalWrite(4, HIGH);
        delay(2000);
        mode = 3;
      Serial.println("unlock");
    }
  }
}

String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));


  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--Electro\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--Electro\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--Electro--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=Electro");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
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


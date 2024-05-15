#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "quirc.h"
#include "time.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Wire.h>
/* ======================================== */
const char *ssid = "Nha Tro Tien Nghi P19";
const char *password = "123456789";
/* ======================================== */
#define I2Ckey            0x3F
#define I2Clcd            0x27
#define Password_Lenght   5
#define SDA               15
#define SCL               14
#define ControlOUT        4
#define CoiBao            16
#define PhimCamUng        13
/* ======================================== */
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
/* ======================================== */
camera_fb_t * fb = NULL;
struct quirc_code codeE;
struct quirc_data dataA;
quirc_decode_error_t err;

String QRCodeResult = "NANN";
/* ======================================== */
unsigned long timee;
unsigned long timeeout = 0;
bool timeSecond = true;



const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'#', '0', '*'}, {'9', '8', '7'}, {'6', '5', '4'}, {'3', '2', '1'}};
char Data[Password_Lenght];
char Data2[Password_Lenght];
char Master[Password_Lenght];
char FistTimePassword[] = {'1', '2', '3', '4'};
char key;
char last_press_key;
byte wifires = 0;
byte data_count = 0;
byte key_state = 0;
byte mode = 4;
byte lockkk[] = {0b01110, 0b10001, 0b10001, 0b11111,
                 0b11011, 0b11011, 0b11111, 0b00000};
byte clockkk[] = {0b01110, 0b10000, 0b10000, 0b11111,
                  0b11011, 0b11011, 0b11111, 0b00000};
byte rowPins[ROWS] = {2, 3, 1, 4};
byte colPins[COLS] = {5, 6, 7};
Keypad_I2C kpd(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2Ckey, PCF8574);
LiquidCrystal_I2C lcd(I2Clcd, 16, 2);
/* ________________________________________________________________________________  */
void Unlockk();
void Lockk();
void collectKey();
void clearData();
void printLocalTime();
void dumpData(const struct quirc_data *dataa);
void qrScan();
void Check_EEPROM();
void configInitCamera();
/* ________________________________________________________________________________  */
void setup() {
  // Disable brownout detector.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  /* ---------------------------------------- Init serial communication speed (baud rate). */
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  /* ---------------------------------------- */
  Wire.begin(SDA, SCL);
  kpd.begin(makeKeymap(keys));
  /* ---------------------------------------- */
  Check_EEPROM();
  Serial.println(Master);
  // khoi dong lcd
  lcd.begin();
  lcd.createChar(0, lockkk);
  lcd.createChar(1, clockkk);
  lcd.home();
  lcd.clear();
  lcd.backlight();
  // config camera
  configInitCamera();
  // khoa (lock)
  pinMode(ControlOUT, OUTPUT);
  digitalWrite(ControlOUT, LOW);
  // chip
  pinMode(CoiBao, OUTPUT);
  digitalWrite(CoiBao, LOW);
  // nut an
  pinMode(PhimCamUng, INPUT);
  // Ket noi Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  lcd.setCursor(2, 0);
  lcd.print("Dang Ket Noi");
  lcd.setCursor(6, 1);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    wifires++;
    delay(500);
    Serial.print(".");
    if (wifires % 4 == 0) {
      lcd.setCursor(6, 1);
      lcd.print("    ");
    } else {
      lcd.setCursor(6, 1);
      lcd.print("wifi");
    }
    if (wifires > 150) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Dang khoi dong");
      lcd.setCursor(10, 1);
      lcd.print("lai!");
      ESP.restart();
    }
  }
  timee = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Da Ket Noi WIFI!");
  Serial.println("CONNECTED to WIFI");
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
  // time
  configTime(25200, 0, "pool.ntp.org");
  delay(1000);
  lcd.clear();
  printLocalTime();
}
/* ________________________________________________________________________________ */
void loop() {
  key = kpd.getKey();
  key_state = kpd.getState();


  if (key) {
    if (key == '#' && mode == 4)                 mode = 5;
    if (key == '*' && mode == 5){ lcd.clear();   mode = 4;
      }else if (mode == 4){       lcd.clear();   mode = 0;}
    last_press_key = key;
    Serial.println(key);
  }
  switch (mode){
    case 0:
        lcd.setCursor(0, 0);
        lcd.print("Nhap Mat Khau:");
        break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("Nhap MKhau Moi");
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print("Nhap Lai MK");
        break;
    case 3:
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(3, 0);
        lcd.print("Da Mo Khoa!");
        break;
    case 4:
        if ((unsigned long)(millis() - timee) > 1000) {
            timee = millis();
            printLocalTime();
    }
        break;
    case 5:
    if (QRCodeResult == "NANN") Serial.println("ok"); //qrScan();
    if (QRCodeResult == "DH32112380_NGUYENTHANHPHU_01.01.2003") Unlockk();
    break;
  }
  if (mode == 3) {
    if (last_press_key == '#' && key_state == 2) mode = 1;
    if (last_press_key == '*' && key_state == 2) Lockk();
    if ((timeeout != 0) && ((unsigned long)(millis() - timeeout) > 1000 * 60)) Lockk();
  }
  if (key && key != '#' && key != '*' && mode != 3 && mode != 5)
    collectKey();

  if (!digitalRead(PhimCamUng)) {
    if ((mode == 0) || (mode == 4)) {
      Unlockk();
      clearData();
    } else if (mode == 3) {
      Lockk();
    }
  }

  if (data_count == Password_Lenght - 1) {
    if (mode == 0) {
      lcd.clear();
      Serial.println(Master);

      if (!strcmp(Data, Master)) {
        Unlockk();
      } else {
        lcd.setCursor(2, 0);
        lcd.print("Mat Khau");
        lcd.setCursor(4, 1);
        lcd.print("Khong Dung!");
        delay(2000);
        lcd.clear();
        mode = 4;
      }
      delay(1000);
      lcd.clear();
      clearData();
    } else if (mode == 1) {
      lcd.clear();
      mode = 2;
      for (int i = 0; i < Password_Lenght; i = i + 1) {
        Data2[i] = Data[i];
      }
      clearData();
    } else if (mode == 2) {
      if (!strcmp(Data, Data2)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MK moi la:");
        lcd.setCursor(4, 1);
        lcd.print(Data);
        delay(2000);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Saving...");
        for (int i = 0; i <= 100; i = i + 1) {
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
      } else {
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
/* ________________________________________________________________________________ */
void dumpData(const struct quirc_data *data)
{
  Serial.printf("Version: %d\n", data->version);
  Serial.printf("ECC level: %c\n", "MLHQ"[data->ecc_level]);
  Serial.printf("Mask: %d\n", data->mask);
  Serial.printf("Length: %d\n", data->payload_len);
  Serial.printf("Payload: %s\n", data->payload);
  QRCodeResult = (const char *)data->payload;
}
void qrScan(){
struct quirc *qr;
uint8_t *image;
  qr = quirc_new();
  fb = esp_camera_fb_get();
  if (!fb)  Serial.println("Camera capture failed");
  quirc_resize(qr, fb->width, fb->height);
  image = quirc_begin(qr, NULL, NULL);
  memcpy(image, fb->buf, fb->len);
  quirc_end(qr);
  
  int count = quirc_count(qr);
  if (count > 0) {
    quirc_extract(qr, 0, &codeE);
    err = quirc_decode(&codeE, &dataA);
    if (err){
      Serial.println("Decoding FAILED");
      QRCodeResult = "NANN";
    } else {
      Serial.printf("Decoding successful:\n");
      dumpData(&dataA);
    } 
    Serial.println();
  } 

  esp_camera_fb_return(fb);
  fb = NULL;
  quirc_destroy(qr);
}

void Unlockk() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(4, 0);
  lcd.print("Xin Chao!");
  digitalWrite(ControlOUT, HIGH);
  delay(1000);
  timeeout = millis();
  lcd.clear();
  mode = 3;
}
void Lockk() {
  QRCodeResult = "NANN";
  timeeout = 0;
  mode = 4;
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Da Khoa");
  digitalWrite(ControlOUT, LOW);
  delay(1000);
}
void collectKey() {
  Data[data_count] = key;
  lcd.setCursor(4 + data_count, 1);
  lcd.print("*");
  digitalWrite(CoiBao, HIGH);
  delay(50);
  digitalWrite(CoiBao, LOW);
  data_count++;
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
}
// print time
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.setCursor(2, 0);
    lcd.print("Loading Time");
    Serial.println("Failed to obtain time");
    return;
  }
  timeSecond = !timeSecond;
  lcd.setCursor(2, 0);
  lcd.print(&timeinfo, "%a/%d/%m/%y");
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(6, 1);
  if (timeSecond) {
    lcd.print(&timeinfo, "%H %M");
  } else {
    lcd.print(&timeinfo, "%H:%M");
  }
}
// check eprom
void Check_EEPROM() {
  EEPROM.begin(12);
  EEPROM.get(0, Master);
  Serial.println(Master);
  if (Master[0] == 0 && Master[1] == 0 && Master[2] == 0 && Master[3] == 0) {
    Serial.println("No EEPROM PASSWORD FOUND");
    EEPROM.put(0, FistTimePassword);
    EEPROM.get(0, Master);
    EEPROM.commit();
  }
}
// config cam
void configInitCamera() {
  Serial.println("Start configuring and initializing the camera...");
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
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  // Drop down frame size for higher initial frame rate
  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA); // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  Serial.println("Configure and initialize the camera successfully.");
  Serial.println();
}

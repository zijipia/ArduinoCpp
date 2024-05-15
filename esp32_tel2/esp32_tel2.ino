#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "UniversalTelegramBot.h"


#define BOT_TOKEN "6029268732:AAF0LAu1RqWU3OW9agZRm3ZmJhNlJqRzEmU"
#define _debug
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

const char* ssid = "Nha Tro Tien Nghi P19";
const char* password = "123456789";

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

long bot_last_check;
int bot_check_interval = 3000;

bool hasMoreData;
camera_fb_t * fb = NULL;

bool hasMoreDataAvailable();
byte* getNextBuffer();
int getBufferLen();

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();

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
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, 0); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  bot.longPoll = 60;
}


bool hasMoreDataAvailable() {
  Serial.println("Has more daa");
  if (hasMoreData) {
    hasMoreData = false;
    return true;
  }

  return false;
}


byte* getNextBuffer() {
  Serial.println("Next Buffer ");
  if (fb)
    return fb->buf;

  return nullptr;
}

int getBufferLen() {
Serial.println("Buffer len");
 if (fb)
   return fb->len;

  return 0;
}

void sendImage(String chat_id) { 
  Serial.println("Sending Image");
  fb = NULL;
  fb = esp_camera_fb_get();
  hasMoreData = true;

  Serial.println(fb->len);
  
  bot.sendPhotoByBinary(chat_id, "image/jpeg", fb->len, hasMoreDataAvailable, nullptr, getNextBuffer, getBufferLen);
  
  esp_camera_fb_return(fb);

}

void loop() {

String chat_id = "6179690055";
sendImage(chat_id);
delay(100000);
}
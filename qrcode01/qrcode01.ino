#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "quirc.h"
/* ======================================== */

// creating a task handle
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
/* ======================================== Variables declaration */
camera_fb_t * fb = NULL;
struct quirc_code codee;
struct quirc_data dataA;
quirc_decode_error_t err;

String QRCodeResult = "NANN";
/* ======================================== */
/* ________________________________________________________________________________  */
void setup() {
  // put your setup code here, to run once:

  // Disable brownout detector.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  /* ---------------------------------------- Init serial communication speed (baud rate). */
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  /* ---------------------------------------- */
  pinMode(4, OUTPUT); 

  /* ---------------------------------------- Camera configuration. */
  configInitCamera();
  Serial.println();
}
/* ________________________________________________________________________________ */
void loop() {
  qrScan();
//temp2
  if (QRCodeResult == "http://en.m.wikipedia.org") digitalWrite(4, HIGH);
  if (QRCodeResult == "DH32112380_NGUYENTHANHPHU_01.01.2003") digitalWrite(4, LOW);
}
void dumpData(const struct quirc_data *data) {
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
    quirc_extract(qr, 0, &codee);
    err = quirc_decode(&codee, &dataA);
    if (err){
      Serial.println("Decoding FAILED");
      QRCodeResult = "NANN";
    } else {
      Serial.printf("OK:\n");
      dumpData(&dataA);
    } 
    Serial.println();
  } 

  esp_camera_fb_return(fb);
  fb = NULL;
  quirc_destroy(qr);
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
  s->set_framesize(s, FRAMESIZE_VGA); // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

  Serial.println("Configure and initialize the camera successfully.");
  Serial.println();
}


struct quirc_code codee;
struct quirc_data dataA;
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
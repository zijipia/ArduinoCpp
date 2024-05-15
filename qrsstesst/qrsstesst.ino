void setup() {
  // put your setup code here, to run once:

}

void loop() {
      qrScan();
      Serial.print

}
void qrScan() {
    struct quirc *qr = quirc_new();
    uint8_t *image;
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }
    quirc_resize(qr, fb->width, fb->height);
    image = quirc_begin(qr, NULL, NULL);
    image = fb->buf;  // Trỏ image trực tiếp đến fb->buf
    quirc_end(qr);
    
    int count = quirc_count(qr);
    if (count > 0) {
        quirc_extract(qr, 0, &codee);
        err = quirc_decode(&codee, &dataA);
        if (err) {
            Serial.println("Decoding FAILED");
            QRCodeResult = "NANN";
        } else {
            Serial.printf("OK:\n");
            QRCodeResult = (const char *)dataA.payload;
        } 
    } else {
        QRCodeResult = "NANN";
    }

    esp_camera_fb_return(fb);
    quirc_destroy(qr);
}
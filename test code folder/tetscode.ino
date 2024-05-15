key = kpd.getKey();
key_state = kpd.getState();

if (key) {
  if (mode == 4 && key != '#' && key != '*'){
    lcd.clear();
    mode = 0;
  }
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
    if ((unsigned long)(millis() - timee) > 1000) {
      timee = millis();
      printLocalTime(1);
    }
    break;
  case 4:
    if ((unsigned long)(millis() - timee) > 1000) {
      timee = millis();
      printLocalTime(0);
    }
    break;
  case 5:
    if (QRCodeResult == "NANN") {
      qrScan();
      break;
    }
    if (QRCodeResult == QRcodePass) {
      Unlockk();
    } else {
      lcd.setCursor(4, 1);
      lcd.print("Sai QR!");
      delay(1000);
      QRCodeResult = "NANN";
      timeeout = millis();
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Quet Ma QR:");
    }
    break;
  case 6:
    if (QRCodeResult == "NANN") {
      qrScan();
    } else {
      lcd.clear();
      writeStringToEEPROM(10, QRCodeResult);
      QRcodePass = readStringFromEEPROM(10);
      QRCodeResult = "NANN";
      mode = 3;
    }
    break;
}

if (last_press_key == '*' && key_state == 2) {
  Lockk();
}

if (mode == 0 && (time2eout != 0) && (unsigned long)(millis() - time2eout) > 1000 * 60) {
  Lockk();
}

if (mode == 3) {
  if (last_press_key == '0' && key_state == 2) {
    mode = 1;
    lcd.clear();
    delay(1000);
  }
  if (last_press_key == '#' && key_state == 2) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Cai Dat QR!");
    lcd.setCursor(3, 1);
    lcd.print("Quet Ma QR:");
    QRCodeResult = "NANN";
    mode = 6;
    delay(1000);
  }
  if ((timeeout != 0) && ((unsigned long)(millis() - timeeout) > 1000 * 60)) {
    Lockk();
  }
}

if (mode == 5) {
  if ((timeeout != 0) && (unsigned long)(millis() - timeeout) > 1000 * 60) {
    Lockk();
  }
  if (last_press_key == '0' && key_state == 2) {
    LEDqr = !LEDqr;
    digitalWrite(LEDPin, LEDqr ? HIGH : LOW);
    delay(1000);
  }
}

if (key && key == '#' && mode == 4) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Quet Ma QR:");
  timeeout = millis();
  mode = 5;
}

if (key && key != '#' && key != '*' && mode != 3 && mode != 5) {
  collectKey();
}

if (!digitalRead(PhimCamUng)) {
  if ((mode == 0) || (mode == 4)) {
    Unlockk();
    clearData();
  } else if (mode == 3) {
    Lockk();
  }
}

#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Pin setup
SoftwareSerial mySerial(2, 3); // RX, TX for fingerprint sensor
Adafruit_Fingerprint finger(&mySerial);

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

const int relayPin = 8;
const int buttonPin = 9;

// Master fingerprint ID
const uint8_t masterID = 1;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  // Initialize the fingerprint sensor
  initializeFingerprintSensor();
  
  // Initialize pin modes
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Van Tay");
  lcd.setCursor(0, 1);
  lcd.print("Mo Khoa");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    handleButtonPress();
  }
  // Check for fingerprint
  getFingerprintID();
  delay(50);
}

void initializeFingerprintSensor() {
  finger.begin(57600);
  for (int attempt = 0; attempt < 3; attempt++) {
    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      return;
    } else {
      Serial.println("Did not find fingerprint sensor :(");
      delay(1000); // Wait a moment before retrying
    }
  }
  lcd.clear();
  lcd.print("Loi Cam Bien");
  while (1) { delay(1); } // Halt the program
}

void handleButtonPress() {
  lcd.clear();
  lcd.print("Cai Dat Van Tay");
  delay(2000);
  displayMenu();
}

void displayMenu() {
  if (!verifyMaster()) {
    lcd.clear();
    lcd.print("Master Denied");
    delay(2000);
    return;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1: Van Tay Chinh");
  lcd.setCursor(0, 1);
  lcd.print("2: Van Tay Phu");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3: Xoa Van Tay");
  delay(3000);
  
  // Wait for button press
  waitForButtonPress();
  
  lcd.clear();
  lcd.print("Chon Tinh Nang");
  
  // Debounce button
  delay(500);
  
  waitForButtonRelease();
  
  // Get option
  int option = getUserOption(0);
  
  switch (option) {
    case 1:
      lcd.clear();
      lcd.print("Dang Ky Chinh");
      delay(2000);
      enrollFinger(masterID);
      break;
    case 2:
      lcd.clear();
      lcd.print("Dang Ky Phu");
      delay(2000);
      enrollSlaveFinger();
      break;
    case 3:
      lcd.clear();
      lcd.print("Xoa Van Tay");
      delay(2000);
      deleteSlaveFinger();
      break;
    default:
      lcd.clear();
      lcd.print("Lua Chon Sai");
      delay(2000);
      break;
  }
}

void waitForButtonPress() {
  while (digitalRead(buttonPin) == HIGH) {}
}

void waitForButtonRelease() {
  while (digitalRead(buttonPin) == LOW) {}
}

int getUserOption( int count = 0) {
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) { // 3 seconds window to count button presses
    if (digitalRead(buttonPin) == LOW) {
      count++;
      lcd.clear();
      lcd.print("So Lan: "); lcd.print(count);
      delay(500); // Debounce delay
      while (digitalRead(buttonPin) == LOW) {} // Wait for button release
    }
  }
  return count;
}

void getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 0);
    lcd.print("Tu Choi Truy Cap");
    return;
  }
  
  // Found a match!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ID: "); lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("Truy Cap OK");
  
  // Open the door
  digitalWrite(relayPin, HIGH);
  delay(5000); // Keep the door open for 5 seconds
  digitalWrite(relayPin, LOW);
  lcd.clear();
}

// Function to enroll a new finger
void enrollFinger(uint8_t id) {
  lcd.clear();
  lcd.print("Dat Van Tay");
  while (!getFingerPrintEnroll(id));
  lcd.clear();
  lcd.print("Dang Ky OK!");
  delay(2000);
  lcd.clear();
}

// Function to enroll a new slave finger
void enrollSlaveFinger() {
  
  int id = 2; // Start from ID 2 for slaves
  while (id <= 127) { // Limited by the sensor's capacity
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      break; // Find the first empty slot
    }
    id++;
  }
  if (id > 127) {
    lcd.clear();
    lcd.print("Bo Nho Day");
    return;
  }
  
  lcd.clear();
  lcd.print("Dat Van Tay");
  while (!getFingerPrintEnroll(id));
  lcd.clear();
  lcd.print("Dang Ky OK!");
  delay(2000);
  lcd.clear();
}

// Function to delete a slave finger
void deleteSlaveFinger() {
  
  lcd.clear();
  lcd.print("Nhap ID Phu");
  delay(2000);
  
  int id = getUserOption(1); // Replace with actual method to get user input
  if (id <= 1 || id > 127) {
    lcd.clear();
    lcd.print("ID Khong Hop Le");
    delay(2000);
    return;
  }
  
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Da Xoa OK!");
  } else {
    lcd.clear();
    lcd.print("Xoa That Bai");
  }
  delay(2000);
  lcd.clear();
}

// Function to verify master fingerprint
bool verifyMaster() {
  lcd.clear();
  lcd.print("Dat Master");
  while (true) {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) continue;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) continue;

    p = finger.fingerSearch();
    if (p != FINGERPRINT_OK) continue;

    if (finger.fingerID == masterID) {
      return true;
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Van Tay Sai");
      delay(2000);
      return false;
    }
  }
}
bool getFingerPrintEnroll(uint8_t id) {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.setCursor(0, 1);
  lcd.print("Dang Cho...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      lcd.setCursor(0, 1);
      lcd.print("Khong Co Van Tay");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.setCursor(0, 1);
      lcd.print("Loi Ket Noi");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.setCursor(0, 1);
      lcd.print("Loi Hinh Anh");
      break;
    default:
      Serial.println("Unknown error");
      lcd.setCursor(0, 1);
      lcd.print("Loi Khong Biet");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.clear();
      lcd.print("Rut Tay Ra");
      delay(2000);
      lcd.clear();
      lcd.print("Dat Lai Lan 2");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return false;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return false;
    default:
      Serial.println("Unknown error");
      return false;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return false;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return false;
    default:
      Serial.println("Unknown error");
      return false;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return false;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return false;
  } else {
    Serial.println("Unknown error");
    return false;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return false;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return false;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return false;
  } else {
    Serial.println("Unknown error");
    return false;
  }

  return true;
}

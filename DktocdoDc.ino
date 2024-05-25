#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define EN_PIN 11
#define IN1_PIN 12
#define IN2_PIN 13
#define BUTTON_DIR_PIN 2
#define BUTTON_INC_PIN 3
#define BUTTON_DEC_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

int speed = 128; // Tốc độ ban đầu (50%)
bool direction = true; // true: forward, false: backward
unsigned long buttonPressTime = 0;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  pinMode(BUTTON_DIR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_INC_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DEC_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  lcd.setCursor(0, 1);
  lcd.print("Dir: FWD");

  updateSpeed();
}

void loop() {
  // Kiểm tra nút đổi chiều
  if (digitalRead(BUTTON_DIR_PIN) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime > 3000) {
      speed = 0;
      updateSpeed();
      delay(500); // Chờ để tránh xung đột
      buttonPressTime = 0;
    }
  } else if (buttonPressTime > 0) {
    direction = !direction;
    updateDirection();
    buttonPressTime = 0;
    delay(500); // Chờ để tránh xung đột
  }

  // Kiểm tra nút tăng tốc
  if (digitalRead(BUTTON_INC_PIN) == LOW) {
    if (speed < 255) {
      speed += 5;
      if (speed > 255) speed = 255;
      updateSpeed();
      delay(200); // Tránh lặp quá nhanh
    }
  }

  // Kiểm tra nút giảm tốc
  if (digitalRead(BUTTON_DEC_PIN) == LOW) {
    if (speed > 0) {
      speed -= 5;
      if (speed < 0) speed = 0;
      updateSpeed();
      delay(200); // Tránh lặp quá nhanh
    }
  }
}

void updateSpeed() {
  analogWrite(EN_PIN, speed);
  lcd.setCursor(7, 0);
  lcd.print("    "); // Xóa giá trị cũ
  lcd.setCursor(7, 0);
  lcd.print(map(speed, 0, 255, 0, 100));
  lcd.print("%");
}

void updateDirection() {
  if (direction) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    lcd.setCursor(5, 1);
    lcd.print("FWD ");
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    lcd.setCursor(5, 1);
    lcd.print("REV ");
  }
}

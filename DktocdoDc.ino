#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define EN_PIN 11
#define IN1_PIN 12
#define IN2_PIN 13

#define BUTTON_DEC_PIN 2
#define BUTTON_DIR_PIN 3
#define BUTTON_INC_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool status = true; // on, off
int speed = 128; // Initial speed (50%)
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
  lcd.print("Dir:");

  updateSpeed();
  updateDirection();
}

void loop() {
  // Check direction change button
  handleDirectionButton();

  // Check speed increment button
  if (digitalRead(BUTTON_INC_PIN) == LOW) {
    if(!status){
    status = true;
    updateDirection();
    }
    changeSpeed(3);
  }

  // Check speed decrement button
  if (digitalRead(BUTTON_DEC_PIN) == LOW) {
    changeSpeed(-3);
  }
}

void handleDirectionButton() {
  if (digitalRead(BUTTON_DIR_PIN) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime > 3000) {
      toggleStatus();
      delay(500);
      buttonPressTime = 0;
    }
  } else if (buttonPressTime > 0) {
    direction = !direction;
    updateDirection();
    buttonPressTime = 0;
    delay(500);
  }
}

void toggleStatus() {
  speed = (speed == 0) ? 128 : 0;
  status = !status;
  updateSpeed();
  updateDirection();
}

void changeSpeed(int change) {
  speed = constrain(speed + change, 0, 255);
  updateSpeed();
  delay(100);
}

void updateSpeed() {
  analogWrite(EN_PIN, speed);
  lcd.setCursor(7, 0);
  lcd.print("     ");
  lcd.setCursor(7, 0);
  if (speed == 0) {
    lcd.print("TAT");
  } else {
    lcd.print(map(speed, 0, 255, 0, 100));
    lcd.print("%");
  }
}

void updateDirection() {
  if (status) {
    digitalWrite(IN1_PIN, direction ? HIGH : LOW);
    digitalWrite(IN2_PIN, direction ? LOW : HIGH);
    lcd.setCursor(5, 1);
    lcd.print(direction ? "THUAN" : "NGICH");
  } else {
    lcd.setCursor(5, 1);
    lcd.print("       ");
  }
}

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define PLAY_PIN 0
#define LIKE_PIN 1
#define PREV_PIN 10
#define NEXT_PIN 2

#define SDA_PIN 5
#define SCL_PIN 6

#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pressedButton = 0;

void setup()   {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(9600);

  delay(250);
  display.begin(I2C_ADDRESS, true);

  pinMode(PLAY_PIN, INPUT);
  pinMode(LIKE_PIN, INPUT);
  pinMode(PREV_PIN, INPUT);
  pinMode(NEXT_PIN, INPUT);

  display.display();
  delay(1000);
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
}

bool getButtonState(int btnId) {
  switch (btnId) {
    case 1:
      return digitalRead(PLAY_PIN) == HIGH;
    case 2:
      return digitalRead(LIKE_PIN) == HIGH;
    case 3:
      return digitalRead(PREV_PIN) == HIGH;
    case 4:
      return digitalRead(NEXT_PIN) == HIGH;
    default:
      return false;
  }
}

int getPressedButton() {
  if (digitalRead(PLAY_PIN)) {
    return 1;
  }
  if (digitalRead(LIKE_PIN)) {
    return 2;
  }
  if (digitalRead(PREV_PIN)) {
    return 3;
  }
  if (digitalRead(NEXT_PIN)) {
    return 4;
  }
  return 0;
}

void loop() {
  if (pressedButton == 0) {
    pressedButton = getPressedButton();
  } else if (!getButtonState(pressedButton)) {
    pressedButton = 0;
  }
  display.clearDisplay();
  display.drawRect(0, 0, display.width(), display.height(), SH110X_WHITE);

  display.setCursor(20, 12);
  display.print("PLAY");
  display.setCursor(84, 12);
  display.print("LIKE");
  display.setCursor(20, 45);
  display.print("PREV");
  display.setCursor(84, 45);
  display.print("NEXT");

  if (pressedButton == 1) {
    display.drawRect(4, 4, 56, 24, SH110X_WHITE);
  }
  if (pressedButton == 2) {
    display.drawRect(68, 4, 56, 24, SH110X_WHITE);
  }
  if (pressedButton == 3) {
    display.drawRect(4, 36, 56, 24, SH110X_WHITE);
  }
  if (pressedButton == 4) {
    display.drawRect(68, 36, 56, 24, SH110X_WHITE);
  }

  display.display();
}

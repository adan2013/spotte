#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define PLAY_BTN_PIN 0
#define LIKE_BTN_PIN 1
#define PREV_BTN_PIN 10
#define NEXT_BTN_PIN 2

#define SDA_PIN 5
#define SCL_PIN 6
#define SCREEN_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_RESET -1

enum KeyboardState {
  KB_NONE = 0,
  KB_PLAY = 1,
  KB_LIKE = 2,
  KB_PREV = 3,
  KB_NEXT = 4
};

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(9600);
  initKeyboard();
  initScreen();
}

void loop() {
  processKeyboard();
}

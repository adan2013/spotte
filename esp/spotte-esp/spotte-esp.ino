#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <uri/UriGlob.h>

enum KeyboardState {
  KB_NONE = 0,
  KB_PLAY = 1,
  KB_LIKE = 2,
  KB_PREV = 3,
  KB_NEXT = 4
};

enum DeviceState {
  DS_INIT,
  DS_SETUP,
  DS_SETUP_COMPLETE,
  DS_CONNECTING,
};

void setup() {
  Serial.begin(9600);
  initKeyboard();
  initScreen();
  loadConfigFromStorage();
}

void loop() {
  processKeyboard();
  processApClients();
}

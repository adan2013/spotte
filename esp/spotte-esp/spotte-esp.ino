#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <uri/UriGlob.h>

#define SDA_PIN 5
#define SCL_PIN 6
#define SCREEN_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_RESET -1

enum class KeyboardState {
  None = 0,
  Play = 1,
  Like = 2,
  Prev = 3,
  Next = 4
};

enum class DeviceState {
  Init,
  Setup,
  SetupComplete,
  FactoryReset,
  ConnectingWiFi,
  Player,
  ConnectionLost,
};

struct StorageStruct {
  bool configured;
  char ssid[40];
  char password[70];
  char token[300];
};

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);
DeviceState state = DeviceState::Init;
StorageStruct config;

void setup() {
  Serial.begin(9600);
  initKeyboard();
  initScreen();
  loadConfigFromStorage();
}

void loop() {
  processKeyboard();
  processApClients();
  monitorWiFiConnection();
  animatePlayerScreen();
}

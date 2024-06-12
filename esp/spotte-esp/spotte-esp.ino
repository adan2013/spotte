#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <uri/UriGlob.h>

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
  ConnectionLostWithPassword,
};

struct StorageStruct {
  bool configured;
  char ssid[40];
  char password[70];
  char token[300];
};

struct ScrollableText {
  char value[150];
  int screenLength;
  int offset;
};

struct PlayerStruct {
  ScrollableText title;
  ScrollableText artist;
  bool repeat;
  bool shuffle;
  bool liked;
  bool paused;
  int trackPosition;
  int trackLength;
  float trackProgress;
};

DeviceState state = DeviceState::Init;
StorageStruct config;
PlayerStruct player;

void setup() {
  Serial.begin(9600);
  initKeyboard();
  initScreen();
  loadConfigFromStorage();
  updatePlayerState();
}

void loop() {
  processKeyboard();
  processApClients();
  monitorWiFiConnection();
  animatePlayerScreen();
}

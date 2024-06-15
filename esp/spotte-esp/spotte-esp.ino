#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <uri/UriGlob.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <base64.h>

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
  LoggingIn,
  FetchingData,
  Player,
  ConnectionLost,
  ConnectionLostWithPassword,
  Error,
};

struct StorageStruct {
  bool configured;
  char ssid[40];
  char password[70];
  char clientId[100];
  char clientSecret[100];
  char refreshToken[300];
};

struct ScrollableText {
  char value[150];
  int screenLength;
  int offset;
};

struct PlayerStruct {
  bool trackLoaded;
  ScrollableText title;
  ScrollableText artist;
  bool repeat;
  bool shuffle;
  bool liked;
  bool paused;
  long trackPosition;
  long trackLength;
};

DeviceState state = DeviceState::Init;
StorageStruct config;
PlayerStruct player;
String spotifyAccessToken;
String errorMsg;

void resetTimer(unsigned long &timer) {
  timer = millis();
}

bool checkTimer(unsigned long &timer, int interval, bool autoReset = true) {
  if (timer + interval <= millis()) {
    if (autoReset) resetTimer(timer);
    return true;
  }
  return false;
}

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

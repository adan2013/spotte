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

enum class ItemType {
  Track,
  Episode,
};

enum class RepeatMode {
  Off,
  RepeatAll,
  RepeatOne,
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
  String trackId;
  ItemType itemType;
  ScrollableText title;
  ScrollableText artist;
  RepeatMode repeat;
  bool shuffle;
  bool liked;
  bool paused;
  unsigned long trackPosition;
  unsigned long trackLength;
};

DeviceState state = DeviceState::Init;
StorageStruct config;
PlayerStruct player;
String errorMsg;

void resetTimer(unsigned long &timer) {
  timer = millis();
}

bool checkTimer(unsigned long &timer, int interval, bool autoReset = true) {
  if (timer == 0) {
    resetTimer(timer);
    return false;
  }
  if (timer + interval <= millis()) {
    if (autoReset) resetTimer(timer);
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Spott-E welcome!");
  initKeyboard();
  initScreen();
  loadConfigFromStorage();
}

void loop() {
  processKeyboard();
  processApClients();
  monitorWiFiConnection();
  keepAccessTokenValid();
  animatePlayerScreen();
}

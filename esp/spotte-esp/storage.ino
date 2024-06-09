struct StorageStruct {
  bool configured;
  char ssid[40];
  char password[70];
  char token[300];
};

Preferences storage;
StorageStruct config;

void openStorage() {
  storage.begin("spotte", false);
}

void closeStorage() {
  storage.end();
}

void loadConfigFromStorage() {
  openStorage();
  storage.getBytes("config", &config, sizeof(config));
  if (config.configured) {
    switchState(DeviceState::ConnectingWiFi);
  } else {
    switchState(DeviceState::Setup);
  }
  closeStorage();
}

void saveConfigToStorage(String ssid, String password, String token) {
  openStorage();
  config.configured = true;
  ssid.toCharArray(config.ssid, 40);
  password.toCharArray(config.password, 70);
  token.toCharArray(config.token, 300);
  storage.putBytes("config", &config, sizeof(config));
  closeStorage();
}

void resetStorage() {
  openStorage();
  config.configured = false;
  storage.putBytes("config", &config, sizeof(config));
  closeStorage();
}
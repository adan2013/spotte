Preferences storage;

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

void saveConfigToStorage(String ssid, String password, String clientId, String clientSecret, String refreshToken) {
  openStorage();
  config.configured = true;
  ssid.toCharArray(config.ssid, 40);
  password.toCharArray(config.password, 70);
  clientId.toCharArray(config.clientId, 100);
  clientSecret.toCharArray(config.clientSecret, 100);
  refreshToken.toCharArray(config.refreshToken, 300);
  storage.putBytes("config", &config, sizeof(config));
  closeStorage();
}

void resetStorage() {
  openStorage();
  config.configured = false;
  storage.putBytes("config", &config, sizeof(config));
  closeStorage();
}
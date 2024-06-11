bool networkRequired = false;

void connectToWiFi() {
  WiFi.disconnect();
  WiFi.begin(config.ssid, config.password);
  while (WiFi.status() != WL_CONNECTED);
  networkRequired = true;
  switchState(DeviceState::Player);
}

void monitorWiFiConnection() {
  if (networkRequired) {
    if (WiFi.status() != WL_CONNECTED) {
      // networkRequired = false;
      // switchState(DeviceState::ConnectionLost);
    }
  }
}
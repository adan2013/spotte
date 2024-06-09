void renderDisplay() {
  display.clearDisplay();
  switch (state) {
    case DeviceState::Init:
      drawLogotype();
      break;
    case DeviceState::Setup:
      printToCenter(display.width() / 2, 0, "SETUP MODE");
      printToLeft(0, 12, "Connect to the Wi-Fi");
      printToLeft(0, 22, "network and configure");
      printToLeft(0, 32, "this device:");
      printToLeft(0, 45, "SSID: Spotte");
      printToLeft(0, 55, "URL: 192.168.10.1:80");
      break;
    case DeviceState::SetupComplete:
      printToCenter(display.width() / 2, 0, "SETUP MODE");
      printToLeft(0, 25, "Setup completed.");
      printToLeft(0, 35, "Press \"Play\" button");
      printToLeft(0, 45, "to restart the device");
      break;
    case DeviceState::FactoryReset:
      printToCenter(display.width() / 2, 0, "FACTORY RESET");
      printToLeft(0, 15, "Do you want to reset");
      printToLeft(0, 25, "this device?");
      printToLeft(0, 45, "Press Play to confirm");
      printToLeft(0, 55, "Press Like to cancel");
      break;
    case DeviceState::ConnectingWiFi:
      drawLogotype("Connecting to Wi-Fi");
      break;
  }
  display.display();
}

void switchState(DeviceState newState) {
  state = newState;
  renderDisplay();
  switch (state) {
    case DeviceState::Setup:
      turnOnAccessPoint();
    case DeviceState::ConnectingWiFi:
      // connect to wifi
      break;
  }
}
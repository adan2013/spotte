DeviceState state = DS_INIT;

void renderDisplay() {
  display.clearDisplay();
  switch (state) {
    case DS_INIT:
      drawLogotype();
      break;
    case DS_SETUP:
      printToCenter(display.width() / 2, 0, "SETUP MODE");
      printToLeft(0, 12, "Connect to the Wi-Fi");
      printToLeft(0, 22, "network and configure");
      printToLeft(0, 32, "this device:");
      printToLeft(0, 45, "SSID: Spotte");
      printToLeft(0, 55, "URL: 192.168.10.1:80");
      break;
    case DS_SETUP_COMPLETE:
      printToCenter(display.width() / 2, 0, "SETUP MODE");
      printToLeft(0, 25, "Setup completed.");
      printToLeft(0, 35, "Press \"Play\" button");
      printToLeft(0, 45, "to restart the device");
      break;
    case DS_CONNECTING:
      drawLogotype("Connecting to Wi-Fi");
      break;
  }
  display.display();
}

void switchState(DeviceState newState) {
  state = newState;
  renderDisplay();
  switch (state) {
    case DS_SETUP:
      turnOnAccessPoint();
    case DS_CONNECTING:
      // connect to wifi
      break;
  }
}
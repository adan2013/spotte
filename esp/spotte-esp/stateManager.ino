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
      printToLeft(0, 35, "Press Play button");
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
    case DeviceState::Player:
      if (player.repeat) display.drawBitmap(73, 0, repeatModeIcon, 16, 16, SH110X_WHITE);
      if (player.shuffle) display.drawBitmap(93, 0, randomModeIcon, 16, 16, SH110X_WHITE);
      display.drawBitmap(113, 0, player.liked ? heartOnIcon : heartOffIcon, 16, 16, SH110X_WHITE);
      printToLeft(player.title.offset, 20, player.title.value);
      printToLeft(player.artist.offset, 32, player.artist.value);
      printToLeft(0, 50, parseTimeValue(player.trackPosition));
      printToRight(display.width(), 50, parseTimeValue(player.trackLength));
      if (player.paused && blinkAnimationFlag) {
        display.fillRect(60, 48, 3, 8, SH110X_WHITE);
        display.fillRect(65, 48, 3, 8, SH110X_WHITE);
      }
      display.drawRect(0, display.height() - 4, display.width(), 4, SH110X_WHITE);
      display.fillRect(0, display.height() - 4, display.width() * getTrackProgressBarValue(), 4, SH110X_WHITE);
      break;
    case DeviceState::ConnectionLost:
    case DeviceState::ConnectionLostWithPassword:
      printToLeft(0, 0, "Wi-Fi connection lost");
      printToLeft(0, 12, "SSID:");
      printToLeft(0, 22, config.ssid);
      printToLeft(0, 32, "Password:");
      printToLeft(0, 42, state == DeviceState::ConnectionLost ? "(Hold Like to reveal)" : config.password);
      printToLeft(0, 55, "Press Play to retry");
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
      connectToWiFi();
      break;
  }
}
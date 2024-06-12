#define PLAY_BTN_PIN 0
#define LIKE_BTN_PIN 1
#define PREV_BTN_PIN 10
#define NEXT_BTN_PIN 2

#define HOLD_THRESHOLD 3000

KeyboardState pressedButton = KeyboardState::None;
bool holdTriggered = false;
unsigned long buttonPressTime = 0;

void handleShortPress(KeyboardState btn) {
  switch (state) {
    case DeviceState::SetupComplete:
      if (btn == KeyboardState::Play) ESP.restart();
      break;
    case DeviceState::FactoryReset:
      if (btn == KeyboardState::Play) {
        resetStorage();
        ESP.restart();
      }
      if (btn == KeyboardState::Like) ESP.restart();
      break;
    case DeviceState::ConnectionLost:
    case DeviceState::ConnectionLostWithPassword:
      if (btn == KeyboardState::Play) switchState(DeviceState::ConnectingWiFi);
      break;
  }
}

void handleLongPress(KeyboardState btn) {
  switch (state) {
    case DeviceState::ConnectionLost:
    if (btn == KeyboardState::Like) switchState(DeviceState::ConnectionLostWithPassword);
      break;
    case DeviceState::ConnectionLostWithPassword:
      if (btn == KeyboardState::Like) switchState(DeviceState::ConnectionLost);
      break;
  }
}

bool getButtonState(KeyboardState id) {
  switch (id) {
    case KeyboardState::Play:
      return digitalRead(PLAY_BTN_PIN) == HIGH;
    case KeyboardState::Like:
      return digitalRead(LIKE_BTN_PIN) == HIGH;
    case KeyboardState::Prev:
      return digitalRead(PREV_BTN_PIN) == HIGH;
    case KeyboardState::Next:
      return digitalRead(NEXT_BTN_PIN) == HIGH;
    default:
      return false;
  }
}

KeyboardState getKeyboardState() {
  if (digitalRead(PLAY_BTN_PIN)) {
    return KeyboardState::Play;
  }
  if (digitalRead(LIKE_BTN_PIN)) {
    return KeyboardState::Like;
  }
  if (digitalRead(PREV_BTN_PIN)) {
    return KeyboardState::Prev;
  }
  if (digitalRead(NEXT_BTN_PIN)) {
    return KeyboardState::Next;
  }
  return KeyboardState::None;
}

void initKeyboard() {
  pinMode(PLAY_BTN_PIN, INPUT);
  pinMode(LIKE_BTN_PIN, INPUT);
  pinMode(PREV_BTN_PIN, INPUT);
  pinMode(NEXT_BTN_PIN, INPUT);
}

bool isResetSequence() {
  return !getButtonState(KeyboardState::Play)
      && getButtonState(KeyboardState::Like)
      && getButtonState(KeyboardState::Prev)
      && !getButtonState(KeyboardState::Next);
}

void processKeyboard() {
  if (pressedButton == KeyboardState::None) {
    pressedButton = getKeyboardState();
    buttonPressTime = millis();
  } else {
    if (getButtonState(pressedButton)) {
      if (!holdTriggered && millis() - buttonPressTime > HOLD_THRESHOLD) {
        holdTriggered = true;
        if (isResetSequence()) {
          switchState(DeviceState::FactoryReset);
        } else {
          handleLongPress(pressedButton);
        }
      }
    } else {
      if (!holdTriggered) handleShortPress(pressedButton);
      pressedButton = KeyboardState::None;
      holdTriggered = false;
    }
  }
}
#define PLAY_BTN_PIN 0
#define LIKE_BTN_PIN 1
#define PREV_BTN_PIN 10
#define NEXT_BTN_PIN 2

#define HOLD_THRESHOLD 3000

KeyboardState pressedButton = KeyboardState::None;
unsigned long buttonPressTime = 0;

void handleShortPress(KeyboardState btn) {
  switch (state) {
    case DeviceState::SetupComplete:
      if (btn == KeyboardState::Play) ESP.restart();
      break;
  }
}

void handleLongPress(KeyboardState btn) {
  // hold event
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

void processKeyboard() {
  if (pressedButton == KeyboardState::None) {
    pressedButton = getKeyboardState();
    buttonPressTime = millis();
  } else if (!getButtonState(pressedButton)) {
    if(millis() - buttonPressTime < HOLD_THRESHOLD) {
      handleShortPress(pressedButton);
    } else {
      handleLongPress(pressedButton);
    }
    pressedButton = KeyboardState::None;
  }
}
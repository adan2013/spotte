#define PLAY_BTN_PIN 0
#define LIKE_BTN_PIN 1
#define PREV_BTN_PIN 10
#define NEXT_BTN_PIN 2

KeyboardState pressedButton = KeyboardState::None;

void initKeyboard() {
  pinMode(PLAY_BTN_PIN, INPUT);
  pinMode(LIKE_BTN_PIN, INPUT);
  pinMode(PREV_BTN_PIN, INPUT);
  pinMode(NEXT_BTN_PIN, INPUT);
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

void processKeyboard() {
  if (pressedButton == KeyboardState::None) {
    pressedButton = getKeyboardState();
  } else if (!getButtonState(pressedButton)) {
    pressedButton = KeyboardState::None;
  }
}
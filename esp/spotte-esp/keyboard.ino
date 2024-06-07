KeyboardState pressedButton = KB_NONE;

void initKeyboard() {
  pinMode(PLAY_BTN_PIN, INPUT);
  pinMode(LIKE_BTN_PIN, INPUT);
  pinMode(PREV_BTN_PIN, INPUT);
  pinMode(NEXT_BTN_PIN, INPUT);
}

bool getButtonState(KeyboardState id) {
  switch (id) {
    case KB_PLAY:
      return digitalRead(PLAY_BTN_PIN) == HIGH;
    case KB_LIKE:
      return digitalRead(LIKE_BTN_PIN) == HIGH;
    case KB_PREV:
      return digitalRead(PREV_BTN_PIN) == HIGH;
    case KB_NEXT:
      return digitalRead(NEXT_BTN_PIN) == HIGH;
    default:
      return false;
  }
}

KeyboardState getKeyboardState() {
  if (digitalRead(PLAY_BTN_PIN)) {
    return KB_PLAY;
  }
  if (digitalRead(LIKE_BTN_PIN)) {
    return KB_LIKE;
  }
  if (digitalRead(PREV_BTN_PIN)) {
    return KB_PREV;
  }
  if (digitalRead(NEXT_BTN_PIN)) {
    return KB_NEXT;
  }
  return KB_NONE;
}

void processKeyboard() {
  if (pressedButton == KB_NONE) {
    pressedButton = getKeyboardState();
  } else if (!getButtonState(pressedButton)) {
    pressedButton = KB_NONE;
  }
}
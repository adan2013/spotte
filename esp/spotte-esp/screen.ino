#define SDA_PIN 5
#define SCL_PIN 6
#define SCREEN_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

int getTextWidth(const char *input) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(input, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void printToLeft(int16_t x, int16_t y, const char *input) {
  display.setCursor(x, y);
  display.print(input);
}

void printToCenter(int16_t x, int16_t y, const char *input) {
  int w = getTextWidth(input);
  display.setCursor(x - w / 2, y);
  display.print(input);
}

void printToRight(int16_t x, int16_t y, const char *input) {
  int w = getTextWidth(input);
  display.setCursor(x - w, y);
  display.print(input);
}

void drawLogotype(const char *extraText = "") {
  display.setTextSize(2);
  display.fillCircle(96, 27, 10, SH110X_WHITE);
  display.setTextColor(SH110X_BLACK);
  printToLeft(91, 20, "E");
  display.setTextColor(SH110X_WHITE);
  printToLeft(25, 20, "SPOTT");
  display.setTextSize(1);
  if (sizeof(extraText) > 0) {
    printToCenter(display.width() / 2, 55, extraText);
  }
}

void initScreen() {
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(250);
  display.begin(SCREEN_ADDRESS, true);
  switchState(DeviceState::Init);
  delay(1000);
}
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);

void initScreen() {
  delay(250);
  display.begin(SCREEN_ADDRESS, true);
  display.clearDisplay();
  display.setTextSize(2);
  display.fillCircle(96, 37, 10, SH110X_WHITE);
  display.setTextColor(SH110X_BLACK);
  display.setCursor(91, 30);
  display.print("E");
  display.setTextColor(SH110X_WHITE);
  display.setCursor(25, 30);
  display.print("SPOTT");
  display.setTextSize(1);
  display.display();
  delay(1000);
}
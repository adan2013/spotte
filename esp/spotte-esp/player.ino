#define ANIMATION_INTERVAL 600
#define SCROLL_STEP 8

unsigned long nextAnimationUpdateTime = 0;
bool blinkAnimationFlag = false;

int titleOffset = 0;
int artistOffset = 0;

int titleScreenLength = 0;
int artistScreenLength = 0;

char title[150] = "Asylums for the feeling feat. Leila Adu";
char artist[150] = "Silent Poets";

void renderPlayer() {
  display.clearDisplay();
  display.drawBitmap(73, 0, repeatModeIcon, 16, 16, SH110X_WHITE);
  display.drawBitmap(93, 0, randomModeIcon, 16, 16, SH110X_WHITE);
  display.drawBitmap(113, 0, heartOffIcon, 16, 16, SH110X_WHITE);

  titleScreenLength = getTextWidth(title);
  artistScreenLength = getTextWidth(artist);
  printToLeft(titleOffset, 20, title);
  printToLeft(artistOffset, 32, artist);

  printToLeft(0, 50, "0:54");
  printToRight(display.width(), 50, "4:31");
  
  if (blinkAnimationFlag) {
    display.fillRect(60, 48, 3, 8, SH110X_WHITE);
    display.fillRect(65, 48, 3, 8, SH110X_WHITE);
  }

  display.drawRect(0, display.height() - 4, display.width(), 4, SH110X_WHITE);
  display.fillRect(0, display.height() - 4, 25, 4, SH110X_WHITE);
  display.display();
}

void animatePlayerScreen() {
  if (state == DeviceState::Player && millis() > nextAnimationUpdateTime) {
    nextAnimationUpdateTime = millis() + ANIMATION_INTERVAL;
    blinkAnimationFlag = !blinkAnimationFlag;
    int16_t w = display.width();
    bool titleOverflow = titleScreenLength > w;
    bool artistOverflow = artistScreenLength > w;
    bool titleReached = false;
    bool artistReached = false;
    if (titleOverflow) {
      titleOffset -= SCROLL_STEP;
      titleReached = titleOffset < titleScreenLength * -1;
    }
    if (artistOverflow) {
      artistOffset -= SCROLL_STEP;
      artistReached = artistOffset < artistScreenLength * -1;
    }
    if ((titleReached || !titleOverflow) && (artistReached || !artistOverflow)) {
      titleOffset = 0;
      artistOffset = 0;
    }
    renderPlayer();
  }
}
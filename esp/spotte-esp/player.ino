#define SCREEN_SCROLL_LIMIT 128
#define ANIMATION_INTERVAL 600
#define SCROLL_STEP 8
#define PLAYER_UPDATE_INTERVAL 3000

unsigned long nextAnimationUpdateTime = 0;
unsigned long nextPlayerUpdateTime = 0;
bool blinkAnimationFlag = false;

float getTrackProgressBarValue() {
  return float(player.trackPosition) / float(player.trackLength);
}

void animatePlayerScreen() {
  if (state != DeviceState::Player) return;
  if (millis() > nextAnimationUpdateTime) {
    nextAnimationUpdateTime = millis() + ANIMATION_INTERVAL;
    blinkAnimationFlag = !blinkAnimationFlag;
    bool titleOverflow = player.title.screenLength > SCREEN_SCROLL_LIMIT;
    bool artistOverflow = player.artist.screenLength > SCREEN_SCROLL_LIMIT;
    bool titleReached = false;
    bool artistReached = false;
    if (titleOverflow) {
      player.title.offset -= SCROLL_STEP;
      titleReached = player.title.offset < player.title.screenLength * -1;
    }
    if (artistOverflow) {
      player.artist.offset -= SCROLL_STEP;
      artistReached = player.artist.offset < player.artist.screenLength * -1;
    }
    if ((titleReached || !titleOverflow) && (artistReached || !artistOverflow)) {
      player.title.offset = 0;
      player.artist.offset = 0;
    }
    renderDisplay();
  }
  if (millis() > nextPlayerUpdateTime) {
    nextPlayerUpdateTime = millis() + PLAYER_UPDATE_INTERVAL;
    updatePlayerState();
  }
}
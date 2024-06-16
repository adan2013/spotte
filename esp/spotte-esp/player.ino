#define SCREEN_SCROLL_LIMIT 128
#define ANIMATION_INTERVAL 1000
#define SCROLL_STEP 10
#define PLAYER_UPDATE_INTERVAL 3000

unsigned long lastAnimationUpdateTime = 0;
unsigned long lastPlayerUpdateTime = 0;
bool blinkAnimationFlag = false;

void resetPlayerState() {
  player.trackLoaded = false;
}

void updatePlayerState(DynamicJsonDocument doc) {
  String type = doc["currently_playing_type"].as<String>();
  JsonObject item = doc["item"];

  String title = "";
  String artist = "";

  if (type == "track") {
    title = item["name"].as<String>();
    JsonArray artists = item["artists"];
    for (JsonObject artistObj : artists) {
      if (artist.length() > 0) {
        artist += ", ";
      }
      artist += artistObj["name"].as<String>();
    }
    player.shuffle = doc["shuffle_state"].as<bool>();
    String rep = doc["repeat_state"].as<String>();
    player.repeat = rep != "null" && rep != "off";
  } else if (type == "episode") {
    title = item["name"].as<String>();
    artist = item["show"]["publisher"].as<String>();
    player.shuffle = false;
    player.repeat = false;
  }

  player.trackLoaded = true;
  player.paused = !doc["is_playing"].as<bool>();
  player.trackPosition = doc["progress_ms"].as<unsigned long>();
  player.trackLength = item["duration_ms"].as<unsigned long>();

  char previousTitle[sizeof(player.title.value)];
  strcpy(previousTitle, player.title.value);
  title.toCharArray(player.title.value, 150);
  artist.toCharArray(player.artist.value, 150);
  if (strcmp(previousTitle, player.title.value) != 0) {
    player.title.screenLength = getTextWidth(player.title.value);
    player.artist.screenLength = getTextWidth(player.artist.value);
    player.title.offset = 0;
    player.artist.offset = 0;
  }
}

float getTrackProgressBarValue() {
  return float(player.trackPosition) / float(player.trackLength);
}

void animatePlayerScreen() {
  if (state != DeviceState::Player) return;
  bool trackEndReached = false;
  if (checkTimer(lastAnimationUpdateTime, ANIMATION_INTERVAL, false)) {
    int deltaTime = millis() - lastAnimationUpdateTime;
    resetTimer(lastAnimationUpdateTime);
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
    if (player.trackLoaded && !player.paused && player.trackLength > 0) {
      player.trackPosition += deltaTime;
      if (player.trackPosition >= player.trackLength) {
        player.trackPosition = player.trackLength;
        trackEndReached = true;
      }
    }
    renderDisplay();
  }
  if (trackEndReached || checkTimer(lastPlayerUpdateTime, PLAYER_UPDATE_INTERVAL)) {
    updatePlayerState();
  }
}

void forcePlayerUpdate() {
  resetTimer(lastPlayerUpdateTime);
  updatePlayerState();
}

void initPlayerScreen() {
  lastAnimationUpdateTime = millis();
  lastPlayerUpdateTime = millis();
}
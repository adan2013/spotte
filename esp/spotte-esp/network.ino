#define TOKEN_REFRESH_INTERVAL 3500000
#define API_TOKEN "https://accounts.spotify.com/api/token"
#define API_PLAYER "https://api.spotify.com/v1/me/player?additional_types=episode"
#define API_PLAY "https://api.spotify.com/v1/me/player/play"
#define API_PAUSE "https://api.spotify.com/v1/me/player/pause"
#define API_PREVIOUS "https://api.spotify.com/v1/me/player/previous"
#define API_NEXT "https://api.spotify.com/v1/me/player/next"
#define API_SEEK "https://api.spotify.com/v1/me/player/seek?position_ms="
#define API_SHUFFLE "https://api.spotify.com/v1/me/player/shuffle?state="
#define API_REPEAT "https://api.spotify.com/v1/me/player/repeat?state="
#define API_TRACK_ADD_REMOVE "https://api.spotify.com/v1/me/tracks?ids="
#define API_TRACK_CONTAINS "https://api.spotify.com/v1/me/tracks/contains?ids="
#define API_EPISODE_ADD_REMOVE "https://api.spotify.com/v1/me/episodes?ids="
#define API_EPISODE_CONTAINS "https://api.spotify.com/v1/me/episodes/contains?ids="

bool monitorNetworkStatus = false;
bool monitorAccessToken = false;
unsigned long lastTokenRefreshTime;
String spotifyAccessToken;

void connectToWiFi() {
  WiFi.disconnect();
  WiFi.begin(config.ssid, config.password);
  while (WiFi.status() != WL_CONNECTED);
  monitorNetworkStatus = true;
  switchState(DeviceState::LoggingIn);
}

void monitorWiFiConnection() {
  if (monitorNetworkStatus) {
    if (WiFi.status() != WL_CONNECTED) {
      monitorNetworkStatus = false;
      switchState(DeviceState::ConnectionLost);
    }
  }
}

void keepAccessTokenValid() {
  if (monitorAccessToken) {
    if (checkTimer(lastTokenRefreshTime, TOKEN_REFRESH_INTERVAL)) {
      if(!refreshAccessToken()) {
        switchState(DeviceState::Error);
      }
    }
  }
}

bool refreshAccessToken() {
  monitorAccessToken = true;
  HTTPClient http;
  String authHeader = String(config.clientId) + ":" + String(config.clientSecret);
  String base64AuthHeader = base64::encode(authHeader);
  String postData = "grant_type=refresh_token&refresh_token=" + String(config.refreshToken);

  http.begin(API_TOKEN);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Basic " + base64AuthHeader);
  int httpResponseCode = http.POST(postData);

  String payload = "";
  if (httpResponseCode == 200) {

    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      errorMsg = "Token JSON deserialization " + String(error.c_str());
      return false;
    }

    spotifyAccessToken = doc["access_token"].as<String>();
    Serial.println(spotifyAccessToken);
    if (doc.containsKey("refresh_token")) {
      saveNewRefreshToken(doc["refresh_token"].as<String>());
    }

    return true;
  } else if (httpResponseCode < 0) {
    errorMsg = "Token response code: " + String(httpResponseCode);
  } else {
    String payload = http.getString();
    errorMsg = "Token response code: " + String(httpResponseCode) + "; Payload: " + payload;
  }
  http.end();
  return false;
}

bool updatePlayerState() {
  HTTPClient http;
  http.begin(API_PLAYER);
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      errorMsg = "Player state JSON deserialization " + String(error.c_str());
      return false;
    }

    updatePlayerState(doc);
    return true;
  } else if (httpResponseCode == 204) {
    resetPlayerState();
    return true;
  } else if (httpResponseCode < 0) {
    errorMsg = "Player state response code: " + String(httpResponseCode);
  } else {
    String payload = http.getString();
    errorMsg = "Player state response code: " + String(httpResponseCode) + "; Payload: " + payload;
  }
  http.end();
  return false;
}

bool triggerAction(String operationName, const char *type, String endpoint) {
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Length", "0");
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = http.sendRequest(type);
  if (httpResponseCode == 200 || httpResponseCode == 204) {
    http.end();
    return true;
  } else if (httpResponseCode < 0) {
    errorMsg = operationName + " response code: " + String(httpResponseCode);
    http.end();
    switchState(DeviceState::Error);
  } else {
    String payload = http.getString();
    errorMsg = operationName + " response code: " + String(httpResponseCode) + "; Payload: " + payload;
    http.end();
    switchState(DeviceState::Error);
  }
  return false;
}

bool triggerPlay() {
  return triggerAction("Play", "PUT", API_PLAY);
}

bool triggerPause() {
  return triggerAction("Pause", "PUT", API_PAUSE);
}

bool triggerPrevious() {
  return triggerAction("Previous", "POST", API_PREVIOUS);
}

bool triggerNext() {
  return triggerAction("Next", "POST", API_NEXT);
}

bool seekToPosition(int diff) {
  long targetTime = player.trackPosition + diff;
  if (targetTime < 0) targetTime = 0;
  return triggerAction("Seek", "PUT", API_SEEK + String(targetTime));
}

bool toggleShuffleMode() {
  String newState = player.shuffle ? "false" : "true";
  return triggerAction("Toggle shuffle", "PUT", API_SHUFFLE + newState);
}

bool toggleRepeatMode() {
  String newState = "";
  switch(player.repeat) {
    case RepeatMode::Off:
      newState = "context";
      break;
    case RepeatMode::RepeatAll:
      newState = "track";
      break;
    default:
      newState = "off";
      break;
  }
  return triggerAction("Toggle repeat", "PUT", API_REPEAT + newState);
}

bool toggleLikeState() {
  if (player.trackLoaded) {
    char method[7];
    if (player.liked) {
      strcpy(method, "DELETE");
    } else {
      strcpy(method, "PUT");
    }
    switch (player.itemType) {
      case ItemType::Track:
        return triggerAction("Toggle track", method, API_TRACK_ADD_REMOVE + player.trackId);
        break;
      case ItemType::Episode:
        return triggerAction("Toggle episode", method, API_EPISODE_ADD_REMOVE + player.trackId);
        break;
    }
  }
  return false;
}

bool checkIsItSaved() {
  HTTPClient http;
  String endpoint = "";
  switch (player.itemType) {
    case ItemType::Track:
      endpoint = API_TRACK_CONTAINS + player.trackId;
      break;
    case ItemType::Episode:
      endpoint = API_EPISODE_CONTAINS + player.trackId;
      break;
  }
  http.begin(endpoint);
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      errorMsg = "LibraryCheck JSON deserialization " + String(error.c_str());
      switchState(DeviceState::Error);
      return false;
    }
    bool isSaved = doc[0];
    http.end();
    return isSaved;
  } else if (httpResponseCode < 0) {
    errorMsg = "LibraryCheck response code: " + String(httpResponseCode);
    http.end();
    switchState(DeviceState::Error);
  } else {
    String payload = http.getString();
    errorMsg = "LibraryCheck response code: " + String(httpResponseCode) + "; Payload: " + payload;
    http.end();
    switchState(DeviceState::Error);
  }
  return false;
}
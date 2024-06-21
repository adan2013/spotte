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
      refreshAccessToken();
    }
  }
}

bool decodeResponse(const String name, HTTPClient &http, DynamicJsonDocument &doc) {
  String payload = http.getString();
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    errorMsg = name + " deserialization error: " + String(error.c_str());
    switchState(DeviceState::Error);
    return false;
  }
  return true;
}

bool validateResponse(const String name, HTTPClient &http, int code, bool errorOnFail = true) {
  if (code >= 200 && code < 300) {
    return true;
  } else if (code < 0) {
    http.end();
    if (errorOnFail) {
      errorMsg = name + " call failed";
      switchState(DeviceState::Error);
    }
  } else {
    String payload = http.getString();
    errorMsg = name + " response code " + code + "; Payload: " + payload;
    http.end();
    switchState(DeviceState::Error);
  }
  return false;
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

  if (validateResponse("Token", http, httpResponseCode)) {
    DynamicJsonDocument doc(1024);
    if (decodeResponse("Token", http, doc)) {
      spotifyAccessToken = doc["access_token"].as<String>();
      Serial.println(spotifyAccessToken);
      if (doc.containsKey("refresh_token")) {
        saveNewRefreshToken(doc["refresh_token"].as<String>());
      }
      http.end();
      return true;
    }
  }
  return false;
}

bool updatePlayerState() {
  HTTPClient http;
  http.begin(API_PLAYER);
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = http.GET();

  if (validateResponse("Player state", http, httpResponseCode, false)) {
    if (httpResponseCode == 204) {
      resetPlayerState();
      http.end();
      return true;
    } else {
      DynamicJsonDocument doc(2048);
      if (decodeResponse("Player state", http, doc)) {
        updatePlayerState(doc);
        http.end();
        return true;
      }
    }
  }
  return false;
}

bool triggerAction(const String operationName, const char *type, String endpoint) {
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Length", "0");
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = http.sendRequest(type);
  bool result = validateResponse(operationName, http, httpResponseCode, false);
  if (result) http.end();
  return result;
}

bool triggerPlay() {
  if (!player.trackLoaded) return false;
  return triggerAction("Play", "PUT", API_PLAY);
}

bool triggerPause() {
  if (!player.trackLoaded) return false;
  return triggerAction("Pause", "PUT", API_PAUSE);
}

bool triggerPrevious() {
  if (!player.trackLoaded) return false;
  return triggerAction("Previous", "POST", API_PREVIOUS);
}

bool triggerNext() {
  if (!player.trackLoaded) return false;
  return triggerAction("Next", "POST", API_NEXT);
}

bool seekToPosition(int diff) {
  if (!player.trackLoaded) return false;
  long targetTime = player.trackPosition + diff;
  if (targetTime < 0) targetTime = 0;
  return triggerAction("Seek", "PUT", API_SEEK + String(targetTime));
}

bool toggleShuffleMode() {
  if (!player.trackLoaded) return false;
  String newState = player.shuffle ? "false" : "true";
  return triggerAction("Toggle shuffle", "PUT", API_SHUFFLE + newState);
}

bool toggleRepeatMode() {
  if (!player.trackLoaded) return false;
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
  if (!player.trackLoaded) return false;
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

  if (validateResponse("Library check", http, httpResponseCode, false)) {
    DynamicJsonDocument doc(1024);
    if (decodeResponse("Library check", http, doc)) {
      http.end();
      return doc[0];
    }
  }
  return false;
}
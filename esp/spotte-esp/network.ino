#define API_TOKEN "https://accounts.spotify.com/api/token"
#define API_PLAYER "https://api.spotify.com/v1/me/player"
#define API_PLAY "https://api.spotify.com/v1/me/player/play"
#define API_PAUSE "https://api.spotify.com/v1/me/player/pause"
#define API_PREVIOUS "https://api.spotify.com/v1/me/player/previous"
#define API_NEXT "https://api.spotify.com/v1/me/player/next"

bool networkRequired = false;

void connectToWiFi() {
  WiFi.disconnect();
  WiFi.begin(config.ssid, config.password);
  while (WiFi.status() != WL_CONNECTED);
  networkRequired = true;
  switchState(DeviceState::LoggingIn);
}

void monitorWiFiConnection() {
  if (networkRequired) {
    if (WiFi.status() != WL_CONNECTED) {
      networkRequired = false;
      switchState(DeviceState::ConnectionLost);
    }
  }
}

bool refreshAccessToken() {
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

bool triggerPlayerAction(String endpoint, bool usePostMethod) {
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Length", "0");
  http.addHeader("Authorization", "Bearer " + spotifyAccessToken);
  int httpResponseCode = usePostMethod ? http.POST("") : http.PUT("");
  if (httpResponseCode == 204) {
    http.end();
    return true;
  } else if (httpResponseCode < 0) {
    errorMsg = "Player action response code: " + String(httpResponseCode);
  } else {
    String payload = http.getString();
    errorMsg = "Player action response code: " + String(httpResponseCode) + "; Payload: " + payload;
  }
}

bool triggerPlay() {
  return triggerPlayerAction(API_PLAY, false);
}

bool triggerPause() {
  return triggerPlayerAction(API_PAUSE, false);
}

bool triggerPrevious() {
  return triggerPlayerAction(API_PREVIOUS, true);
}

bool triggerNext() {
  return triggerPlayerAction(API_NEXT, true);
}
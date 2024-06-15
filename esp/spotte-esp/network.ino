#define API_TOKEN "https://accounts.spotify.com/api/token"
#define API_PLAYER "https://api.spotify.com/v1/me/player/currently-playing"

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
      errorMsg = "token JSON deserialization";
      return false;
    }

    spotifyAccessToken = doc["access_token"].as<String>();
    Serial.println(spotifyAccessToken);
    if (doc.containsKey("refresh_token")) {
      saveNewRefreshToken(doc["refresh_token"].as<String>());
    }

    return true;
  } else {
    errorMsg = "token response code: " + String(httpResponseCode);
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
      errorMsg = "player JSON deserialization";
      return false;
    }

    updatePlayerState(doc);
    return true;
  } else if (httpResponseCode == 204) {
    resetPlayerState();
    return true;
  } else {
    errorMsg = "player response code: " + String(httpResponseCode);
  }
  http.end();
  return false;
}
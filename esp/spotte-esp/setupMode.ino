DNSServer dnsServer;
IPAddress apIP(192, 168, 10, 1);
IPAddress netMask(255, 255, 255, 0);

WebServer server(80);
String header;

void turnOnAccessPoint() {
  WiFi.softAPConfig(apIP, apIP, netMask);
  WiFi.softAP("Spotte");
  delay(500);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);
  server.on("/", handleSetupRequest);
  server.on("/submit", handleSubmitRequest);
  server.on(UriGlob("/generate_204*"), handleSetupRequest);
  server.on("/fwlink", handleSetupRequest);
  server.onNotFound(handleSetupRequest);
  server.begin();
}

void handleSetupRequest() {
  String html = "<html lang=\"en\"><head><title>Spotte</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>html { font-family: Helvetica, 'sans-serif'; } h1 { text-align: center; } input { width: 100%; padding: 5px; } button { padding: 8px; }</style></head>";
  html += "<body><h1>SPOTT-E</h1><form method=\"get\" action=\"/submit\">";
  html += "<p>Wi-Fi SSID:<br /><input type=\"text\" name=\"ssid\" /></p>";
  html += "<p>Wi-Fi password:<br /><input type=\"text\" name=\"pass\" /></p>";
  html += "<p>Spotify access token:<br /><input type=\"text\" name=\"auth\" /></p>";
  html += "<p>How to get the access token?<br />Go back to your normal Wi-Fi network, open <a href=\"https://google.com\">this link</a> and follow the instruction.</p>";
  html += "<button type=\"submit\">Save and initialize</button></form></body></html>";
  server.send(200, "text/html", html); 
}

void handleSubmitRequest() {
  String html = "<html lang=\"en\"><head><title>Spotte</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>html { font-family: Helvetica, 'sans-serif'; }</style></head>";
  html += "<body><p>Configuration saved. Check the screen of the Spotte device to see new instructions.</p></body></html>";
  server.send(200, "text/html", html);
  saveConfigToStorage(server.arg("ssid"), server.arg("pass"), server.arg("auth"));
  switchState(DS_SETUP_COMPLETE);
}

void processApClients() {
  dnsServer.processNextRequest();
  server.handleClient();
}

#define SSID_CONNECT_MAX_MILLIS       10000
#define NETWORK_MODE_SWITCH_PIN       18




const char* ssid = "StoffelNetwork-5";
const char* password = "9529471171";
int switchState = 0;
int prevSwitchState = switchState;

//WiFiServer server(80);
AsyncWebServer server(80);

void (*customAuxFunc[])(void) { // define custom auxiliary functions here
  [] { Serial.println("running customAuxFunc[0]"); },
  [] { Serial.println("running customAuxFunc[1]"); },
  [] { Serial.println("running customAuxFunc[2]"); }
};


void wifiSetup() {

    otaSetup();
//    attachInterrupt(digitalPinToInterrupt(NETWORK_MODE_SWITCH_PIN), CHANGE);

    pinMode(NETWORK_MODE_SWITCH_PIN, INPUT);
    switchState = digitalRead(NETWORK_MODE_SWITCH_PIN);
    if (switchState == HIGH) {
      connectToNetworkSetup();
    } else {
      apSetup();
    }

    restOfNetworkSetup();
   

   
    
}


void connectToNetworkSetup() {

  WiFi.mode(WIFI_MODE_STA);
  WiFi.enableAP(false);
  WiFi.softAPdisconnect();
  WiFi.softAPdisconnect(false);

  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");

      switchState = digitalRead(NETWORK_MODE_SWITCH_PIN);
      if (switchState != prevSwitchState) {
        return;
      }
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
}

void apSetup() {
      // Connect to Wi-Fi network with SSID and password
        Serial.print("Setting AP (Access Point)…");
        // Remove the password parameter, if you want the AP (Access Point) to be open
        
//        WiFi.softAP("PandaLights", "somepass");
        WiFi.softAP("PandaLights");
      
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
}

void restOfNetworkSetup() {

   // Set up mDNS responder:
    // - first argument is the domain name, in this example
    //   the fully-qualified domain name is "esp8266.local"
    // - second argument is the IP address to advertise
    //   we send our IP address on the WiFi network
    if (!MDNS.begin("PandaLighting")) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }

    
    
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);



    serverHandleSetup();
// Start TCP (HTTP) server
    server.begin();
    Serial.println("TCP server started");



  
}

void otaSetup() {
  // init OTA
  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA end");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}




void serverHandleSetup() {

  server.onNotFound([](AsyncWebServerRequest *request) {

    AsyncWebServerResponse *response = request->beginResponse(404, "text/plain", "Page not found");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    server.send(404, "text/plain", "Page not found");
  });

  // return the WS2812FX status
  // optionally set the running state or run custom auxiliary functions
  server.on("/status", [](AsyncWebServerRequest *request) {
//    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (request->hasParam("running")) {
      String running = request->getParam("running")->value();

      if (running.length() > 0) {
        if (running == "true") ws2812fx.start();
        else ws2812fx.stop();
      }
    }

    if (request->hasParam("auxFunc")) {

      String auxFunc = request->getParam("auxFunc")->value();
      if (auxFunc.length() > 0) {
        int auxFuncIndex = auxFunc.toInt();
        if (auxFuncIndex >= 0 && auxFuncIndex < sizeof(customAuxFunc) / sizeof(customAuxFunc[0])) {
          customAuxFunc[auxFuncIndex]();
        }
      }
    }

    char status[50] = "{\"version\":\"";
    strcat(status, VERSION);
    strcat(status, "\",\"isRunning\":");
    strcat(status, ws2812fx.isRunning() ? "true" : "false");
    strcat(status, "}");


    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", status);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    
//    server.send(200, "application/json", status);
  });

  // send the WS2812FX mode info in JSON format
  server.on("/getModes", [](AsyncWebServerRequest *request) {
    char modes[2000] = "[";
    for (uint8_t i = 0; i < ws2812fx.getModeCount(); i++) {
      strcat(modes, "\"");
      strcat_P(modes, (PGM_P)ws2812fx.getModeName(i));
      strcat(modes, "\",");
    }
    modes[strlen(modes) - 1] = ']';

//    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", modes);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

//    server.send(200, "application/json", modes);
  });

  server.on("/upload", HTTP_OPTIONS, [](AsyncWebServerRequest *request) { // CORS preflight request


    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    request->send(response);
    
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
//    server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
//    server.send(200, "text/plain", "OK");
  });

  // receive the device info in JSON format and update the pattern data
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    String data = request->getParam("plain")->value();
    Serial.println(data);

    bool isParseOk = json2patterns(data);

    if (isParseOk && numPatterns > 0) {
      ws2812fx.stop();
//      ws2812fx.clear();
      ws2812fx.resetSegments();

      saveToEEPROM();

      currentPattern = 0;
      lastTime = 0;
      ws2812fx.start();
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":200, \"message\":\"OK\"}");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

//    server.send(200, "application/json", "{\"status\":200, \"message\":\"OK\"}");
  });
}

  




void wifiLoop() {

//    Watch for turning on AP mode
    switchState = digitalRead(NETWORK_MODE_SWITCH_PIN);
    if (switchState != prevSwitchState) {
      if (switchState == HIGH) {
        connectToNetworkSetup();
      } else {
        apSetup();
      }
      
    }
    digitalWrite(27, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(27, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second




  
}

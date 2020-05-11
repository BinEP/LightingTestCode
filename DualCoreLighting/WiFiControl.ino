

const char* ssid = "StoffelNetwork-5";
const char* password = "9529471171";

//WiFiServer server(80);
AsyncWebServer server(80);


const char* PARAM_MESSAGE = "message";
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void wifiSetup() {
   // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

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

    // Start TCP (HTTP) server
    server.begin();

    serverHandleSetup();

    Serial.println("TCP server started");

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    sharedVar = 1;

//    wifiSetupDone = true;
    while(true);

    
}




void serverHandleSetup() {

//    server.on("/", rootResponse); //Associate the handler function to the path
//    server.on("/base", baseCase); //Associate the handler function to the path


    server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String response = "<html>";

        response += "<head>"
      "<link rel='apple-touch-icon' sizes='128x128' href='https://cdn4.iconfinder.com/data/icons/design-26/24/rgb_red_green_blue_color_monitoe_light_optical-512.png'>"
      "<style>"
      ".button {"
      "  background-color: #4CAF50;"
      "  border: none;"
      "  color: white;"
      "  padding: 15px 32px;"
      "  text-align: center;"
      "  text-decoration: none;"
      "  display: inline-block;"
      "  font-size: 350%;"
      "  margin: 4px 2px;"
      "  cursor: pointer;"
      "}"
      "</style>"
      "<script>"
      "function sendEffect(link, elem) {"
      " elem.style.background = 'blue';"
      " const Http = new XMLHttpRequest();"
      " const url=link;"
      " Http.open('GET', url);"
      " window.setTimeout('greenColor()',2000);"
      " Http.send();"
      "}"
      "function greenColor() {"
      "  var elements = document.getElementsByClassName('button');"
      "  for(var i = 0; i < elements.length; i++){"
      "    elements[i].style.backgroundColor = '#4CAF50';"
      "  }"
      "}"
      "</script>"
      
      
      "</head><body>";
      
      
      //  for (int i = 0; i < effectDataLength; i++) {
      //    response += "<button class='button' onclick='sendEffect(\"http://";
      //    response += host;
      //    response += ".local";
      //    response += "/case?case=";
      //    response += effectData[0][i];
      //    response += "\", this)'>";
      //    response += effectData[1][i];
      //    response += "</button>";
      //  }
        response += "Your IP is: ";
        response += (String) WiFi.localIP();
        response += "</body></html>";
        
      
        request->send(200, "text/html", response);
        Serial.println(response);
        Serial.print("response() running on core ");
        Serial.println(xPortGetCoreID());

        changeLEDs = true;
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

     // Send a GET request to <IP>/get?message=<message>
    server.on("/random", HTTP_GET, [] (AsyncWebServerRequest *request) {
        changeRandom = true;
        request->send(200, "text/plain", "Hello, Setting to Random");
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });


  
}



void wifiLoop() {


    digitalWrite(27, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(27, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second




  
}

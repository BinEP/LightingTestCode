#include <WiFi.h>
#include <ESPmDNS.h>
#include <HttpRequest.h>

const char* ssid = "StoffelNetwork-5";
const char* password = "9529471171";

WiFiServer server(80);
HttpRequest httpReq;

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
    Serial.println("TCP server started");

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    
}



void wifiLoop() {


   // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    
    Serial.println("");
    Serial.println("New client");

    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
       
        //parse the received caracter
        httpReq.parseRequest(c);
        
        Serial.write(c);
        
        //IF request has ended -> handle response
        if (httpReq.endOfRequest()) {


          
            char name[HTTP_REQ_PARAM_NAME_LENGTH], value[HTTP_REQ_PARAM_VALUE_LENGTH];

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<body>");
        
            //access object properties
            client.print("Method: ");
            client.print(httpReq.method);
            client.println("<br>");
            client.print("Uri: ");
            client.print(httpReq.uri);
            client.println("<br>");
            client.print("Version: ");
            client.print(httpReq.version);
            client.println("<br>");
            client.print("paramCount: ");
            client.print(httpReq.paramCount);
            client.println("<br>");
            //list received parameters GET and POST
             client.println("Parameters:<br>");
            for(int i=1;i<=httpReq.paramCount;i++){
              httpReq.getParam(i,name,value);
              client.print(name);
              client.print("-");
              client.print(value);
              client.println("<br>");
            }
              /*
             //find a particular parameter name
              pos=httpReq.getParam("style",value);
              if(pos>0){
                client.print("<br>");
                client.print("Found 'style'. Value: ");
                client.print(value);
                ledMode[index] = atoi(value) + 1;
                clearStripBeforehand = true;
                printLEDModes();
                client.println("<br>");  
              }*/
            
            client.println("</body>");
            client.print("</html>");

        }
      }
    }

    client.stop();    
    Serial.println("Done with client");


    httpReq.resetRequest();
   
    digitalWrite(27, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(27, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a second




  
}

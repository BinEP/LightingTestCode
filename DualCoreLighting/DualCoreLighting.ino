#include <WS2812FX.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Preferences.h>


#include <ESPmDNS.h>

#define VERSION "2.1.0"

#define LED_COUNT 390 
#define LED_PIN 12

#define TIMER_MS 5000

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[LED_COUNT + 1];
WS2812FX ws2812fx = WS2812FX(leds, LED_COUNT);


TaskHandle_t ledControl;
//TaskHandle_t wifiSetupTask;

volatile bool changeLEDs = false;
volatile bool changeRandom = false;


void setup() {

  otaSetup();
  delay(1000);
  Serial.begin(115200);
  pinMode(27, OUTPUT);

  ws2812fx.start();
  wifiSetup();

  ws2812fx.addLeds<LED_PIN>(0, 30);
  ws2812fx.addLeds<13>(30, 330);
  ws2812fx.addLeds<25>(330, 390);
  ws2812fx.init();
  ws2812fx.setBrightness(255);
//  ws2812fx.setSpeed(1000);
//  ws2812fx.setColor(0x007BFF);
//  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);

  
  ws2812fx.setSegment(0, 0, 30, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(1, 30, 180, FX_MODE_TWINKLE_NATURE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(2, 180, 330, FX_MODE_THEATER_CHASE_CANDY, 0xFF0000, 1000, false);
  ws2812fx.setSegment(3, 330, 352, FX_MODE_BLINK_RAINBOW, 0xFF0000, 1000, false);
  ws2812fx.setSegment(4, 352, 390, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);

  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());



  
  xTaskCreatePinnedToCore(
      ledLoop, /* Function to implement the task */
      "LED Control", /* Name of the task */
      3000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      100,  /* Priority of the task Brady here: I want highest priority for timing sensitive elements*/
      &ledControl,  /* Task handle. */
      0); /* Core where the task should run */




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

void loop() {

  ArduinoOTA.handle();

  wifiLoop();

}

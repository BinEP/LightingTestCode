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
Preferences ledPreferences;


TaskHandle_t ledControl;
//TaskHandle_t wifiSetupTask;

volatile bool changeLEDs = false;
volatile bool changeRandom = false;


void setup() {

  delay(1000);
  Serial.begin(115200);
  pinMode(27, OUTPUT);

  
  
 
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  
  xTaskCreatePinnedToCore(
      ledLoopParent, /* Function to implement the task */
      "LED Control", /* Name of the task */
      3000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      100,  /* Priority of the task Brady here: I want highest priority for timing sensitive elements*/
      &ledControl,  /* Task handle. */
      0); /* Core where the task should run */


  wifiSetup();


}



void loop() {

  ArduinoOTA.handle();

  wifiLoop();

}

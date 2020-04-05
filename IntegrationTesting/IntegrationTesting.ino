#define FASTLED_ESP32_I2S


#include <WS2812FX.h>

#include <FastLED.h>

#define NUM_LEDS 10
#define NUM_STRIPS 4


CRGB leds[NUM_LEDS];
//CLEDController *controllers[NUM_STRIPS];

WS2812FX ws2812fx = WS2812FX(leds, NUM_LEDS);



void setup() {
  // put your setup code here, to run once:
  ws2812fx.init();
  
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(200);
  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  ws2812fx.service();
}

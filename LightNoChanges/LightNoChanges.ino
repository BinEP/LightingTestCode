#include <WS2812FX.h>

#define LED_COUNT 30000 
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


unsigned long last_change = 0;
unsigned long now = 0;

void setup() {

  ws2812fx.addLeds<LED_PIN>(0, 15000);
  ws2812fx.addLeds<13>(15000, 15000);
  ws2812fx.init();
  ws2812fx.setBrightness(255);
//  ws2812fx.setSpeed(1000);
//  ws2812fx.setColor(0x007BFF);
//  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);

  
  ws2812fx.setSegment(0, 0, 15, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(1, 15, 30, FX_MODE_FLASH_SPARKLE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(2, 30, LED_COUNT, FX_MODE_STATIC, 0xFF0000, 1000, false);
  ws2812fx.start();
}

void loop() {
  now = millis();

  ws2812fx.service();

  if(now - last_change > TIMER_MS) {
    ws2812fx.setMode(0, (ws2812fx.getMode(0) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(1, (ws2812fx.getMode(1) + 1) % ws2812fx.getModeCount());
    last_change = now;
  }
}

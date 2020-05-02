#include <WS2812FX.h>

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


unsigned long last_change = 0;
unsigned long now = 0;

void setup() {

  Serial.begin(115200);

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
  ws2812fx.setSegment(3, 330, 360, FX_MODE_BLINK_RAINBOW, 0xFF0000, 1000, false);
  ws2812fx.setSegment(4, 360, 390, FX_MODE_FIRE_FLICKER_INTENSE, 0xFF0000, 1000, false);
  ws2812fx.start();



   for (int i = 0; i < 256; i += 8) {
    uint32_t ogCol = ws2812fx.color_wheel(i);
    CRGB myCol = ws2812fx.fadeColorScheme(i, 4);

    Serial.print(ogCol, HEX);
    Serial.print("\t");
    Serial.print(myCol, HEX);
    Serial.print("\t");
    Serial.print(myCol.green, HEX);
    Serial.print(", ");
    Serial.print(myCol.red, HEX);
    Serial.print(", ");
    Serial.print(myCol.blue, HEX);
    Serial.println();
    
   }
}

void loop() {
  now = millis();

  ws2812fx.service();

  if(now - last_change > TIMER_MS) {
    ws2812fx.setMode(0, (ws2812fx.getMode(0) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(1, (ws2812fx.getMode(1) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(2, (ws2812fx.getMode(2) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(3, (ws2812fx.getMode(3) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(4, (ws2812fx.getMode(4) + 1) % ws2812fx.getModeCount());
    last_change = now;
  }

  
 
  



  
}

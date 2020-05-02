#include <WS2812FX.h>

#define LED_COUNT 330 
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

volatile int sharedVar = 0;


unsigned long last_change = 0;
unsigned long now = 0;


TaskHandle_t wifiControl;
TaskHandle_t wifiSetupTask;

void setup() {

  delay(1000);
  Serial.begin(115200);
  pinMode(27, OUTPUT);

   xTaskCreatePinnedToCore(
      wifiSetup, /* Function to implement the task */
      "Setup", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &wifiSetupTask,  /* Task handle. */
      0); /* Core where the task should run */

//  wifiSetup();

  ws2812fx.addLeds<LED_PIN>(0, 30);
  ws2812fx.addLeds<13>(30, 330);
  ws2812fx.init();
  ws2812fx.setBrightness(255);
//  ws2812fx.setSpeed(1000);
//  ws2812fx.setColor(0x007BFF);
//  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);

  
  ws2812fx.setSegment(0, 0, 30, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(1, 30, 180, FX_MODE_TWINKLE_NATURE, 0xFF0000, 1000, false);
  ws2812fx.setSegment(2, 180, LED_COUNT, FX_MODE_THEATER_CHASE_CANDY, 0xFF0000, 5000, false);
  ws2812fx.start();

  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());


  xTaskCreatePinnedToCore(
      wifiControlLoop, /* Function to implement the task */
      "Blink", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &wifiControl,  /* Task handle. */
      0); /* Core where the task should run */
}


void wifiControlLoop(void * pvParameters ) {
  
//  Serial.print("blink() running on core ");
//  Serial.println(xPortGetCoreID());
//  
//  for (;;) {
//    digitalWrite(27, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(1000);                       // wait for a second
//    digitalWrite(27, LOW);    // turn the LED off by making the voltage LOW
//    delay(1000);                       // wait for a second
//  }
  Serial.print("Wifi() running on core ");
  Serial.println(xPortGetCoreID());
  for(;;) {
    wifiLoop();
  }
}

void loop() {
  now = millis();

  ws2812fx.service();

  if(now - last_change > TIMER_MS) {
    ws2812fx.setMode(0, (ws2812fx.getMode(0) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(1, (ws2812fx.getMode(1) + 1) % ws2812fx.getModeCount());
    ws2812fx.setMode(2, (ws2812fx.getMode(2) + 1) % ws2812fx.getModeCount());
    last_change = now;
  }

//  Serial.print("loop() running on core ");
//  Serial.println(xPortGetCoreID());
 
  



  
}

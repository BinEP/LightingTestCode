//runs in it's own little world
//On it's own core. This core should only be LEDs because timing sensitive
//Everything else can happen elsewhere
#define MAX_NUM_PATTERNS 10

#define DEFAULT_PATTERN {30, 64, 1, { {0, LED_COUNT-1, LED_COUNT*20, FX_MODE_STATIC, NO_OPTIONS, {RED,  BLACK, BLACK}} }}

typedef struct Pattern { // 208 bytes/pattern
  int duration;
  uint8_t brightness;
  uint8_t numSegments;
  WS2812FX::segment segments[MAX_NUM_SEGMENTS];
} pattern;

// setup a default patterns array
Pattern patterns[MAX_NUM_PATTERNS] = { DEFAULT_PATTERN };
int numPatterns = 1;
int currentPattern = 0;
unsigned long lastTime = 0;



void ledLoop(void * parameter) {

  ws2812fx.service();

  // if it's time to change pattern, do it now
  unsigned long now = millis();
  if (lastTime == 0 || (now - lastTime > patterns[currentPattern].duration * 1000)) {
//    ws2812fx.clear();
    ws2812fx.resetSegments();

    currentPattern = (currentPattern + 1) % numPatterns;
    ws2812fx.setBrightness(patterns[currentPattern].brightness);
    for (int i = 0; i < patterns[currentPattern].numSegments; i++) {
      WS2812FX::segment seg = patterns[currentPattern].segments[i];
      ws2812fx.setSegment(i, seg.start, seg.stop, seg.mode, seg.colors, seg.speed, seg.options);
    }
    lastTime = now;
  }

//  if (changeLEDs) {
//      long effect = random(0, 105);
//      ws2812fx.setMode(0, effect);
//      ws2812fx.setMode(1, effect);
//      ws2812fx.setMode(2, effect);
//      ws2812fx.setMode(3, effect);
//      ws2812fx.setMode(4, effect);
//    
//      last_change = now;
//      changeLEDs = false;
//  } else if (changeRandom) {
//      long effect = random(0, 105);
//      ws2812fx.setMode(0, effect);
//      effect = random(0, 105);
//      ws2812fx.setMode(1, effect);
//      effect = random(0, 105);
//      ws2812fx.setMode(2, effect);
//      effect = random(0, 105);
//      ws2812fx.setMode(3, effect);
//      effect = random(0, 105);
//      ws2812fx.setMode(4, effect);
//      changeRandom = false;
//  } else {
//    if(now - last_change > TIMER_MS) {
//
//      ws2812fx.setMode(0, (ws2812fx.getMode(0) + 1) % ws2812fx.getModeCount());
//      ws2812fx.setMode(1, (ws2812fx.getMode(1) + 1) % ws2812fx.getModeCount());
//      ws2812fx.setMode(2, (ws2812fx.getMode(2) + 1) % ws2812fx.getModeCount());
//      ws2812fx.setMode(3, (ws2812fx.getMode(3) + 1) % ws2812fx.getModeCount());
//      ws2812fx.setMode(4, (ws2812fx.getMode(4) + 1) % ws2812fx.getModeCount());
//    }
//    
//    last_change = now;
//  }

//  Serial.print("loop() running on core ");
//  Serial.println(xPortGetCoreID());
 
  



  
}

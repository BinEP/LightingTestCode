//runs in it's own little world
//On it's own core. This core should only be LEDs because timing sensitive
//Everything else can happen elsewhere
#define MAX_NUM_PATTERNS 10
#define MAX_NUM_SCENES   1
//                       duration, brightness, numSegments, [ { first, last, speed, mode, options, colors[] } ]
#define DEFAULT_PATTERN {30, 150, 1, { {0, LED_COUNT-1, LED_COUNT*20, FX_MODE_RAINBOW_CYCLE, NO_OPTIONS, {RED,  BLACK, BLACK}} }}

//TODO add scenes
typedef struct Pattern { // 208 bytes/pattern
  int duration;
  uint8_t brightness;
  uint8_t numSegments;
  WS2812FX::segment segments[MAX_NUM_SEGMENTS];
} pattern;

typedef struct Scene { // 208 bytes/pattern
  int numPatterns = 1;
  Pattern patterns[MAX_NUM_PATTERNS] = { DEFAULT_PATTERN };
} board_scene;

// setup a default patterns array
//Remove this, replace with scenes. Set patterns var in each function
//Pattern patterns[MAX_NUM_PATTERNS] = { DEFAULT_PATTERN };
Scene scenes[MAX_NUM_SCENES];
int currentScene = 0;
int numScenes = 1;

int currentPattern = 0;
unsigned long lastTime = 0;



void ledLoopParent(void * parameter) {

  ws2812fx.addLeds<LED_PIN>(0, 30);
  ws2812fx.addLeds<13>(30, 330);
  ws2812fx.addLeds<25>(330, 390);
  ws2812fx.init();
  ws2812fx.setBrightness(150);
//  ws2812fx.setSpeed(1000);
//  ws2812fx.setColor(0x007BFF);
//  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);

  
//  ws2812fx.setSegment(0, 0, 30, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);
//  ws2812fx.setSegment(1, 30, 180, FX_MODE_TWINKLE_NATURE, 0xFF0000, 1000, false);
//  ws2812fx.setSegment(2, 180, 330, FX_MODE_THEATER_CHASE_CANDY, 0xFF0000, 1000, false);
//  ws2812fx.setSegment(3, 330, 352, FX_MODE_BLINK_RAINBOW, 0xFF0000, 1000, false);
//  ws2812fx.setSegment(4, 352, 390, FX_MODE_RAINBOW_CYCLE, 0xFF0000, 1000, false);


   ledPreferences.begin("prefs", false);
//    Get previous settings and load
     
     
     // if not rebooting due to catastrophic error, restore pattern data from eeprom
//    struct  rst_info  *rstInfo = system_get_rst_info();
    //Serial.print("rstInfo->reason:"); Serial.println(rstInfo->reason);
//    if (rstInfo->reason !=  REASON_EXCEPTION_RST) { // not reason 2
      restoreFromEEPROM();
//    }


  ws2812fx.start();
  while(true) {
    ledLoop();
  }
}

void ledLoop() {

  ws2812fx.service();

  // if it's time to change pattern, do it now
  Pattern* patterns = scenes[currentScene].patterns;
  int numPatterns = scenes[currentScene].numPatterns;

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
}

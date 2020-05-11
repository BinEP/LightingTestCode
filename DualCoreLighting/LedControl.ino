//runs in it's own little world
//On it's own core. This core should only be LEDs because timing sensitive
//Everything else can happen elsewhere

void ledLoop(void * parameter) {
  now = millis();

  ws2812fx.service();

  if (changeLEDs) {
      long effect = random(0, 105);
      ws2812fx.setMode(0, effect);
      ws2812fx.setMode(1, effect);
      ws2812fx.setMode(2, effect);
      ws2812fx.setMode(3, effect);
      ws2812fx.setMode(4, effect);
    
      last_change = now;
      changeLEDs = false;
  } else if (changeRandom) {
      long effect = random(0, 105);
      ws2812fx.setMode(0, effect);
      effect = random(0, 105);
      ws2812fx.setMode(1, effect);
      effect = random(0, 105);
      ws2812fx.setMode(2, effect);
      effect = random(0, 105);
      ws2812fx.setMode(3, effect);
      effect = random(0, 105);
      ws2812fx.setMode(4, effect);
      changeRandom = false;
  } else {
    if(now - last_change > TIMER_MS) {

      ws2812fx.setMode(0, (ws2812fx.getMode(0) + 1) % ws2812fx.getModeCount());
      ws2812fx.setMode(1, (ws2812fx.getMode(1) + 1) % ws2812fx.getModeCount());
      ws2812fx.setMode(2, (ws2812fx.getMode(2) + 1) % ws2812fx.getModeCount());
      ws2812fx.setMode(3, (ws2812fx.getMode(3) + 1) % ws2812fx.getModeCount());
      ws2812fx.setMode(4, (ws2812fx.getMode(4) + 1) % ws2812fx.getModeCount());
    }
    
    last_change = now;
  }

//  Serial.print("loop() running on core ");
//  Serial.println(xPortGetCoreID());
 
  



  
}

int modeName2Index(const char* name) {
  for (int i = 0; i < ws2812fx.getModeCount(); i++) {
    if (strcmp_P(name, (PGM_P)ws2812fx.getModeName(i)) == 0) {
      return i;
    }
  }
  return 0;
}


void saveToEEPROM() {
  Serial.println("saving to Preferences");
  
  ledPreferences.putInt("numScenes", numScenes);
  ledPreferences.putBytes("scenes", scenes, sizeof(scenes));
//  EEPROM.put(sizeof(int) * 0, (int)EEPROM_MAGIC_NUMBER);
//  EEPROM.put(sizeof(int) * 1, (int)ws2812fx.getPin());
//  EEPROM.put(sizeof(int) * 2, (int)ws2812fx.getLength());
//  EEPROM.put(sizeof(int) * 3, (int)numPatterns);
//  EEPROM.put(sizeof(int) * 4, patterns);
//  EEPROM.commit(); // for ESP8266 (comment out if using an Arduino)
}

void restoreFromEEPROM() {

    numScenes = ledPreferences.getInt("numScenes", 1);
    size_t numBytes = ledPreferences.getBytesLength("scenes");
    if (numBytes != 0) {
       ledPreferences.getBytes("scenes", scenes, sizeof(scenes));
    }
    
}



#if ARDUINOJSON_VERSION_MAJOR == 5
//#pragma message("Compiling for ArduinoJson v5")
bool json2patterns(String &json, int index) {

  Pattern* patterns = scenes[index].patterns;
  int numPatterns = scenes[index].numPatterns;
  DynamicJsonBuffer jsonBuffer(2000);
  JsonObject& deviceJson = jsonBuffer.parseObject(json);
  if (deviceJson.success()) {
//    ws2812fx.setPin(deviceJson["dataPin"]);
//    ws2812fx.setLength(deviceJson["numLeds"]);

    JsonArray& patternsJson = deviceJson["patterns"];
    if (patternsJson.size() > 0 ) {
      numPatterns = 0;
      for (int i = 0; i < patternsJson.size(); i++) {
        JsonObject& patt = patternsJson[i];
//      bool isEnabled = patt["isEnabled"];
//      if (! isEnabled) continue; // disabled patterns are not stored

        JsonArray& segmentsJson = patt["segments"];
        if (segmentsJson.size() == 0 ) continue;

        patterns[numPatterns].brightness = patt["brightness"];
        patterns[numPatterns].duration = patt["duration"];

        patterns[numPatterns].numSegments = segmentsJson.size();
        for (int j = 0; j < segmentsJson.size(); j++) {
          JsonObject& seg = segmentsJson[j];
          //seg.printTo(Serial);Serial.println();
          int start = seg["start"];
          if (start < 0 || start >= ws2812fx.getLength()) start = 0;
          patterns[numPatterns].segments[j].start = start;

          int stop = seg["stop"];
          if (stop < 0 || stop >= ws2812fx.getLength()) stop = ws2812fx.getLength() - 1;
          patterns[numPatterns].segments[j].stop = stop;

          if (seg["mode"].is<unsigned int>()) { // seg["mode"] can be a mode number or a mode name
            patterns[numPatterns].segments[j].mode = seg["mode"];
          } else {
            patterns[numPatterns].segments[j].mode = modeName2Index(seg["mode"]);
          }

          int speed = seg["speed"];
          if (speed < SPEED_MIN || speed >= SPEED_MAX) speed = 1000;
          patterns[numPatterns].segments[j].speed = speed;

          patterns[numPatterns].segments[j].options = 0;
          bool reverse = seg["reverse"];
          if (reverse) patterns[numPatterns].segments[j].options |= REVERSE;

          bool gamma = seg["gamma"];
          if (gamma) patterns[numPatterns].segments[j].options |= GAMMA;

          int fadeRate = seg["fadeRate"];
          if (fadeRate > 0) patterns[numPatterns].segments[j].options |= (fadeRate & 0x7) << 4;

          int size = seg["size"];
          if (size > 0) patterns[numPatterns].segments[j].options |= (size & 0x3) << 1;

          JsonArray& colors = seg["colors"]; // the web interface sends three color values
          // convert colors from strings ('#ffffff') to uint32_t
          patterns[numPatterns].segments[j].colors[0] = strtoul(colors[0].as<char*>() + 1, 0, 16);
          patterns[numPatterns].segments[j].colors[1] = strtoul(colors[1].as<char*>() + 1, 0, 16);
          patterns[numPatterns].segments[j].colors[2] = strtoul(colors[2].as<char*>() + 1, 0, 16);
        }
        numPatterns++;
        if (numPatterns >= MAX_NUM_PATTERNS) break;
      }
    } else {
      Serial.println(F("JSON contains no pattern data"));
      return false;
    }
  } else {
    Serial.println(F("Could not parse JSON payload"));
    return false;
  }
  return true;
}
#endif

#if ARDUINOJSON_VERSION_MAJOR == 6
//#pragma message("Compiling for ArduinoJson v6")
bool json2patterns(String &json, int index) {
  // in ArduinoJson v6 a DynamicJsonDocument does not expand as needed
  // like it did in ArduinoJson v5. So rather then try to compute the
  // optimum heap size, we'll just allocated a bunch of space on the
  // heap and hope it's enough.
  int freeHeap = ESP.getFreeHeap();
  Pattern* patterns = scenes[currentScene].patterns;
  int numPatterns = scenes[index].numPatterns;

  DynamicJsonDocument doc(freeHeap - 3096); // allocate all of the available heap except 3kB
  DeserializationError error = deserializeJson(doc, json);
  if (!error) {
    JsonObject deviceJson = doc.as<JsonObject>();
//    ws2812fx.setPin(deviceJson["dataPin"]);
//    ws2812fx.setLength(deviceJson["numLeds"]);

    JsonArray patternsJson = deviceJson["patterns"];
    if (patternsJson.size() > 0 ) {
      numPatterns = 0;
      for (int i = 0; i < patternsJson.size(); i++) {
        JsonObject patt = patternsJson[i];
//      bool isEnabled = patt["isEnabled"];
//      if (! isEnabled) continue; // disabled patterns are not stored

        JsonArray segmentsJson = patt["segments"];
        if (segmentsJson.size() == 0 ) continue;

        patterns[numPatterns].brightness = patt["brightness"];
        patterns[numPatterns].duration = patt["duration"];

        patterns[numPatterns].numSegments = segmentsJson.size();
        for (int j = 0; j < segmentsJson.size(); j++) {
          JsonObject seg = segmentsJson[j];
//seg.printTo(Serial);Serial.println();

          int start = seg["start"];
          if (start < 0 || start >= ws2812fx.getLength()) start = 0;
          patterns[numPatterns].segments[j].start = start;

          int stop = seg["stop"];
          if (stop < 0 || stop >= ws2812fx.getLength()) stop = ws2812fx.getLength() - 1;
          patterns[numPatterns].segments[j].stop = stop;

          if (seg["mode"].is<unsigned int>()) { // seg["mode"] can be a mode number or a mode name
            patterns[numPatterns].segments[j].mode = seg["mode"];
          } else {
            patterns[numPatterns].segments[j].mode = modeName2Index(seg["mode"]);
          }

          int speed = seg["speed"];
          if (speed < SPEED_MIN || speed >= SPEED_MAX) speed = 1000;
          patterns[numPatterns].segments[j].speed = speed;

          patterns[numPatterns].segments[j].options = 0;
          bool reverse = seg["reverse"];
          if (reverse) patterns[numPatterns].segments[j].options |= REVERSE;

          bool gamma = seg["gamma"];
          if (gamma) patterns[numPatterns].segments[j].options |= GAMMA;

          int fadeRate = seg["fadeRate"];
          if (fadeRate > 0) patterns[numPatterns].segments[j].options |= (fadeRate & 0x7) << 4;

          int size = seg["size"];
          if (size > 0) patterns[numPatterns].segments[j].options |= (size & 0x3) << 1;

          JsonArray colors = seg["colors"]; // the web interface sends three color values
          // convert colors from strings ('#ffffff') to uint32_t
          patterns[numPatterns].segments[j].colors[0] = strtoul(colors[0].as<char*>() + 1, 0, 16);
          patterns[numPatterns].segments[j].colors[1] = strtoul(colors[1].as<char*>() + 1, 0, 16);
          patterns[numPatterns].segments[j].colors[2] = strtoul(colors[2].as<char*>() + 1, 0, 16);
        }
        numPatterns++;
        if (numPatterns >= MAX_NUM_PATTERNS)  break;
      }
    } else {
      Serial.println(F("JSON contains no pattern data"));
      return false;
    }
  } else {
    Serial.print(F("Could not parse JSON payload: "));
    Serial.println(error.c_str());
    return false;
  }
  return true;
}
#endif

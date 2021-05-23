/*
  Library for creating a device that 'runs' LED strips. It takes input from
  a serial (or other?) connection as a pile of bytes, and maps that to a
  connected LED strip.

  We assume running on fancy hardware that doesn't mind some floating point,
  assuming that communication (both serial input and LED output) dominate,
  and that memory is also plentiful...

  Also assuming that we (potentially) have a white channel...

  The library has a collection of behaviours that can be run to do different
  things. It expects three kinds of packet:
  - setup packets configure strip functions etc.
  - behaviour packets choose a behaviour and specify parameters
  - data packets are used to update
*/

#ifndef GlowController_h
#define GlowController_h
#include "Arduino.h"
#include "GlowBehaviours.h"
#include "GlowStrip.h"
#include <ArduinoJson.h>
#include <OctoWS2811.h>
#include <EEPROM.h>

const int MAX_BEHAVIOURS = 30;

class GlowController {
public:
  GlowController(GlowStrip* s, bool blnk=false) : strip(s), behaviours(), blank(blnk) {};
  void runBehaviours();
  //void initialise(GlowStrip* s);
  void processInput(DynamicJsonDocument d);
  void createBehaviour(int id, DynamicJsonDocument d);
  void updateBehaviour(int id, DynamicJsonDocument d);

  void setBehaviour(int i, GlowBehaviour* b) {
    behaviours[i] = b;
    b->start();
  };

  void removeBehaviour(int i) {
    if( behaviours[i] ) behaviours[i]->stop();
    behaviours[i] = NULL;
  };

  void storeColor(JsonDocument d,int address=0) {
    if(d["r"]) tmpColor.r = d["r"];
    if(d["g"]) tmpColor.g = d["g"];
    if(d["b"]) tmpColor.b = d["b"];
    if(d["w"]) tmpColor.w = d["w"];
    storeColor(tmpColor,address);
  }
  void storeColor(FRGBW color,int address=0) {
    Serial.print("Storing color: [");
    strip->printColor(color);
    Serial.println("]");
    EEPROM.put(address,color);
  }
  FRGBW loadColor(FRGBW color,int address=0) {
    FRGBW tColor;
    EEPROM.get(address,tColor);
    Serial.print("Loaded color: [");
    strip->printColor(tColor);
    Serial.println("]");
    return tColor;
  }

protected:
  GlowStrip *strip;
  unsigned long last_update = 0;
  GlowBehaviour* behaviours[MAX_BEHAVIOURS];
  bool blank;
  FRGBW tmpColor;
};



#endif

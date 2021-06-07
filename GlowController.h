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
#include "TimeKeeping.h"
#include <ArduinoJson.h>
//#include <OctoWS2811.h>
#include <EEPROM.h>
#include <TimeLib.h>




const static int MAX_BEHAVIOURS = 20;

class GlowBehaviour;

class GlowController {
public:
  GlowController(GlowStrip* s, bool blnk=false);
  void runBehaviours();
  //void initialise(GlowStrip* s);

  void createBehaviour(JsonVariant d);
  void createBehaviours(JsonVariant d);
  GlowBehaviour* makeBehaviourFromType(const char* name);
  GlowBehaviour* getBehaviour(int id) {return behaviours[id];};

  /* Externally callable methods to send in JSON */
  void update(); // From Serial
  void update(const char* input); //From a string
  void update(byte* input, unsigned int length); //From bytes and length


  void processInput(DynamicJsonDocument d);
  void updateBehaviour(int id, JsonVariant d);

  void setBehaviour(int i, GlowBehaviour* b);
  void removeBehaviour(int i);

  void activateBehaviour(int i);
  void deActivateBehaviour(int i);

  void sendState();
  DynamicJsonDocument outputState();

  void storeColor(JsonVariant d,int address=0) {
    if(d["r"]) tmpColor.r = d["r"];
    if(d["g"]) tmpColor.g = d["g"];
    if(d["b"]) tmpColor.b = d["b"];
    if(d["w"]) tmpColor.w = d["w"];
    storeColor(tmpColor,address);
  }
  void storeColor(FRGBW color,int address=0) {
    Serial.print("Storing color: []");
    //strip->printColor(color);
    //Serial.println("]");
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
  GlowStrip* getStrip() {return strip;}
  TimeKeeping* timeKeeping() { return &time; }

protected:
  GlowStrip *strip;
  unsigned long last_update = 0;
  GlowBehaviour* behaviours[MAX_BEHAVIOURS];
  bool blank;
  FRGBW tmpColor;
  float frameRate;
  DynamicJsonDocument doc;
  FRGBW defaultColor;
  bool checkDeserialisation( DeserializationError error );
  TimeKeeping time;
};



#endif

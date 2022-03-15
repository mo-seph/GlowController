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
#include "BaseController.h"
#include "behaviours/GlowBehaviours.h"
#include "strip/GlowStrip.h"
#include <ArduinoJson.h>
#include <LinkedList.h>
#include <TimeStruct.h>


const static int MAX_BEHAVIOURS = 20;

class GlowBehaviour;
//class GlowFeature;

/*
Init would be:
c.initialise();
c.addBaseFeatures(...)
c.setupControls(...)
c.setupBehaviours(...)
*/


class GlowController : public BaseController {
public:
  GlowController(GlowStrip* s, const char* id, const char* name); 
  virtual void extraLoopCode();
  virtual bool processInput(JsonVariant d);
  virtual DynamicJsonDocument createOutputState();

  void runBehaviours();


  void setupBehaviours(const char* input=NULL ) {
    Serial.println(F("------+ Loading Behaviours +-------"));
    DynamicJsonDocument initialState(8000);
    if( getConfig(initialState, input, "/conf.json") ) {
      processInput(initialState.as<JsonVariant>());
    } else {
      Serial.println("!!! Problem loading Behaviours");
    }
    Serial.println(F("------+ Done Behaviours +-------"));
  }

  void createBehaviour(JsonVariant d);
  void createBehaviours(JsonVariant d);
  GlowBehaviour* makeBehaviourFromType(const char* name);
  GlowBehaviour* getBehaviour(int id) {return behaviours[id];};


  void updateBehaviour(int id, JsonVariant d);

  void setBehaviour(int i, GlowBehaviour* b);
  void removeBehaviour(int i);

  void activateBehaviour(int i);
  void deActivateBehaviour(int i);


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
  void setupInitialColor(); 
  GlowStrip* getStrip() {return strip;}


protected:

  GlowStrip *strip;
  GlowBehaviour* behaviours[MAX_BEHAVIOURS];
  FRGBW tmpColor;
  //DynamicJsonDocument doc;
  FRGBW defaultColor;
};




#endif

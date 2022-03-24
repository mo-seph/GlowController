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


  void storeColor(FRGBW color,const char* key) {
    //Serial.print("Saving color to: "); Serial.println(key);
    color.toSerial();
    prefs.putBytes(key,&color,sizeof(FRGBW));
    //FRGBW check(0,0,0,0);
    //prefs.getBytes(key,&check,sizeof(FRGBW));
    //Serial.println("Check:");
    //check.toSerial();
  }
  bool loadColor(FRGBW& color,const char* key) {
    //Serial.print("Loading color from: "); Serial.println(key);
    prefs.getBytes(key,&color,sizeof(FRGBW));
    color.toSerial();
    return true;
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

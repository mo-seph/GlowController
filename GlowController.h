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
#include "GlowInterfaces.h"
#include "GlowStrip.h"
#include "TimeKeeping.h"
#include <ArduinoJson.h>
//#include <OctoWS2811.h>
#include <EEPROM.h>
//#include <TimeLib.h>
#include <LinkedList.h>
#include <TimeStruct.h>



const static int MAX_BEHAVIOURS = 20;

class GlowBehaviour;


class GlowController {
public:
  GlowController(GlowStrip* s, const char* id, const char* name);
  void runBehaviours();
  //void initialise(GlowStrip* s);

  void createBehaviour(JsonVariant d);
  void createBehaviours(JsonVariant d);
  GlowBehaviour* makeBehaviourFromType(const char* name);
  GlowBehaviour* getBehaviour(int id) {return behaviours[id];};


  void loop();

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
  DynamicJsonDocument createOutputState();

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
  const char* getID() {return id;}
  const char* getName() {return name;}

  TimeKeeping* timeKeeping() { return &time; }
  void addFeature(Feature *f) {
    f->setController(this);
    features.add(f);
  }
  void updateFeatures() {
    int l = features.size();
    for( int i = 0; i < l; i++ ) { features.get(i)->update(); }
  }
  void addConnector(Connector *f) {
    f->setController(this);
    connectors.add(f);
  }
  void updateConnectors() {
    int l = connectors.size();
    for( int i = 0; i < l; i++ ) { connectors.get(i)->update(doc); }
  }

  JsonVariant pingDoc() { return ping_doc.as<JsonVariant>();}

  void ping() {
    Serial.println("Pinging!");
    int l = connectors.size();
    for( int i = 0; i < l; i++ ) { connectors.get(i)->ping(pingDoc()); }
  }

  void setTime(int hour, int minute, int second) {
    if(hour >= 0) current_time.hour = hour;
    if(minute >= 0 ) current_time.minute = minute;
    if( second >= 0 ) current_time.second = second;
  }

  void setDate(int year, int month, int day) {
    if(year >= 0 ) current_time.year = year;
    if( month >= 0 ) current_time.month = month;
    if( day >= 0 ) current_time.day = day;
  }
  CurrentTime* getTime() { return &current_time; }


protected:
  GlowStrip *strip;
  const char* id;
  const char* name;
  unsigned long last_update = 0;
  GlowBehaviour* behaviours[MAX_BEHAVIOURS];
  FRGBW tmpColor;
  float frameRate;
  DynamicJsonDocument doc;
  FRGBW defaultColor;
  bool checkDeserialisation( DeserializationError error );
  TimeKeeping time;
  LinkedList<Feature*> features;
  LinkedList<Connector*> connectors;
  CurrentTime current_time;
  DynamicJsonDocument ping_doc;
};




#endif

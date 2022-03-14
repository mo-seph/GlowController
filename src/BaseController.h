/*
  Base Networked Controller for ESP32 boards
*/

#ifndef BaseController_h
#define BaseController_h
#include "Arduino.h"
#include "connectors/GlowConnectors.h"
#include "features/GlowFeature.h"
#include "TimeKeeping.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
//#include <TimeLib.h>
#include <LinkedList.h>
#include <TimeStruct.h>

class BaseController {
public:
  BaseController(const char* id, const char* name);

  // Main loop
  virtual void loop();
  // Extra behaviour for the middle of the loop
  virtual void extraLoopCode() {};
  // Main message that takes in JSON
  virtual bool processInput(JsonVariant d);

  /* Externally callable methods to send in JSON */
  void update(); // From Serial
  void update(const char* input); //From a string
  void update(byte* input, unsigned int length); //From bytes and length


  /* And to send it out... */
  void sendState();
  virtual DynamicJsonDocument createOutputState();


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
    for( int i = 0; i < l; i++ ) { connectors.get(i)->update(); }
  }

  JsonVariant pingDoc() { return ping_doc.as<JsonVariant>();}

  void ping() {
    Serial.println("Pinging!");
    int l = connectors.size();
    for( int i = 0; i < l; i++ ) { connectors.get(i)->ping(pingDoc()); }
  }

  void setTime(int hour, int minute, int second) {
    hasTime = true;
    if(hour >= 0) current_time.hour = hour;
    if(minute >= 0 ) current_time.minute = minute;
    if( second >= 0 ) current_time.second = second;
  }

  void setDate(int year, int month, int day) {
    hasTime = true;
    if(year >= 0 ) current_time.year = year;
    if( month >= 0 ) current_time.month = month;
    if( day >= 0 ) current_time.day = day;
  }
  CurrentTime* getTime() { return &current_time; }
  bool timeSet() { return hasTime; }


protected:
  const char* id;
  const char* name;
  unsigned long last_update = 0;
  float frameRate;
  bool checkDeserialisation( DeserializationError error );
  TimeKeeping time;
  LinkedList<Feature*> features;
  LinkedList<Connector*> connectors;
  CurrentTime current_time;
  bool hasTime = false;
  DynamicJsonDocument ping_doc;

};




#endif

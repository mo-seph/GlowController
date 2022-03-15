/*
  Base Networked Controller for ESP32 boards
*/

#ifndef BaseController_h
#define BaseController_h
#include "Arduino.h"
#include "connectors/GlowConnectors.h"
#include "controls/ControlManager.h"
#include "features/GlowFeature.h"
//#include "connectors/GlowMQTT.h"
#include "TimeKeeping.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
//#include <TimeLib.h>
#include <LinkedList.h>
#include <TimeStruct.h>
#include <SPIFFS.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

static WiFiClient esp;
static PubSubClient client(esp);


/*
Init would be:
c.initialise();
c.addBaseFeatures(...)
c.setupControls(...)
*/

class Controls;

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


  /* Utility functions */
  const char* getID() {return id;}
  const char* getName() {return name;}

  /* Initialisation */
  virtual void initialise() {
    timeKeeping()->logState = 0;
    timeKeeping()->logTime = 0;
  }

  void setupBaseFeatures(   const char* ssid, const char* password, 
    const char* mqtt_server, int mqtt_port,
    int gmtOffset, int daylightOffset,const char* ntp_server);
  void setupControls(const char* input );

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


  bool getConfig(JsonDocument& target, const char* data, const char* filename) {
    bool isOK = false;
    if( data != NULL ) {
      DeserializationError error =  deserializeJson(target, data);
      if( error ) {
        Serial.print(F("deserializeJson() failed from string: ")); Serial.println(error.f_str()); 
      }
      else { isOK = true; }
    }
    else {
      if( ! SPIFFS.exists(filename)) {
        Serial.print(F("File missing: ")); Serial.println(filename);
      } else {
        File f = SPIFFS.open(filename, "r");
        DeserializationError error = deserializeJson(target, f);
        if( error ) {
          Serial.print(F("deserializeJson() failed from file ")); Serial.print(filename); Serial.print(": "); Serial.println(error.f_str()); 
        }
        else { isOK = true; }
      }
    }
    return isOK;
  }


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
  DynamicJsonDocument controlsSetup;
 

};




#endif

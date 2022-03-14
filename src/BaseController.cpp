#include "GlowController.h"
//#include "behaviours/GlowBehaviours.h"
//#include "features/GlowFeature.h"
//#include "Arduino.h"
//#include "connectors/GlowMQTT.h"
//#include "behaviours/PixelCountdown.h"
//#include "behaviours/ColorAlarm.h"



/*
Example JSON docs:

{"update":0, "data":{"r":0.4,"g":0.3,"b":0.2,"w":0.6}}
{"update":0, "data":{"r":0.9,"g":0.0,"b":0.0,"w":0.0}}
{"update":0, "data":{"r":0.0,"g":0.9,"b":0.0,"w":0.0}}


*/

BaseController::BaseController( const char* id, const char* name) :
  id(id), name(name), frameRate(25.0),  ping_doc(500) {
    setTime(13,11,00);
    setDate(2021,05,29);
    ping_doc["id"] = id;
    ping_doc["name"] = name;
}

void BaseController::loop() {
  time.loopStart();
  time.inputStart();
  updateFeatures();
  updateConnectors();
  time.inputDone();

  extraLoopCode();

  time.showDone();
  time.loopDone();
  time.printOutput();
  time.delayIfNeeded();
}

bool BaseController::processInput(JsonVariant d) {
  bool processed = false;
  Serial.println("Controller processing document:");
  serializeJson(d,Serial);
  if( d.containsKey("setTime")) {
    JsonVariant t = d["setTime"];
    int y = t["year"] | -1;
    int mn = t["month"] | -1;
    int dy = t["day"] | -1;
    int h = t["second"] | -1;
    int m = t["minute"] | -1;
    int s = t["second"] | -1;
    setTime(h,m,s);
    setDate(dy,mn,y);
    processed = true;
  }
  return processed;
}

void BaseController::sendState() {
  Serial.println(F("Controller outputting state"));
  DynamicJsonDocument output = createOutputState();
  JsonVariant opv = output.as<JsonVariant>();
  int l = connectors.size();
  for( int i = 0; i < l; i++ ) {
    connectors.get(i)->outputState(opv);
  }
}

DynamicJsonDocument BaseController::createOutputState() {
  DynamicJsonDocument output(4096);
  output.clear();
  output["id"] = id;
  return output;
}

bool BaseController::checkDeserialisation(DeserializationError error) {
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  } else {
    Serial.println("Got JSON input");
    return true;
  }
}





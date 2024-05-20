#include "GlowController.h"
//#include "behaviours/GlowBehaviours.h"
#include "time.h"
#include "features/GlowFeature.h"
#include "features/NTPFeature.h"
#include "features/WiFiFeature.h"
#include "connectors/SerialConnector.h"
//#include "Arduino.h"
#include "connectors/GlowMQTT.h"
//#include "behaviours/PixelCountdown.h"
//#include "behaviours/ColorAlarm.h"



/*
Example JSON docs:

{"update":0, "data":{"r":0.4,"g":0.3,"b":0.2,"w":0.6}}
{"update":0, "data":{"r":0.9,"g":0.0,"b":0.0,"w":0.0}}
{"update":0, "data":{"r":0.0,"g":0.9,"b":0.0,"w":0.0}}


*/

/*
Things for init in base:
- setupBaseFeatures()
- getConfig( ... controls )
Things for init in glow:
- setupBehaviours()
- initColor()
*/

BaseController::BaseController( const char* id, const char* name) :
  id(id), name(name), frameRate(25.0),  ping_doc(500), controlsSetup(2000) {
    Serial.begin(115200);
    while(!Serial && millis() < 1000 ) {}
    Serial.print("Base Controller created with id "); Serial.print(id); Serial.print(", and name: "); Serial.println(name);
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
  Serial.println();
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
  if( d.containsKey("saveState")) {
    saveState();
    processed = true;
  }
  if( d.containsKey("loadState")) {
    loadState();
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

void BaseController::saveState(const char* filename) {
  Serial.print(F("Controller saving state to ")); Serial.println(filename);
  DynamicJsonDocument output = createOutputState();
  Serial.println("Output: ");
  serializeJson(output, Serial);
  Serial.println("Opening file");
  File file = LittleFS.open(filename, FILE_WRITE);
  Serial.println("Writing to file");
  serializeJson(output, file);
  Serial.print("Written. Closing...");
  file.close();
  Serial.println("Done!");
}

void BaseController::loadState(const char* filename) {
  Serial.print(F("------+ Loading Saved State from '"));
  Serial.print(filename);
  Serial.println(F("' +-------"));
  if(!LittleFS.exists(filename)) {
    Serial.print("File does not exist to load state from: "); Serial.println(filename);
  } else {
    DynamicJsonDocument stateInput(4096);
    bool ok = getConfig(stateInput, NULL, filename);
    if( ok ) {
      Serial.println("Read saved state!");
      serializeJsonPretty(stateInput, Serial);
      Serial.println();
      processFullStateUpdate(stateInput);
      sendState();
    }
  }
  Serial.println(F("------+ Done Loading State +-------"));
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

/*
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
*/

void BaseController::setupBaseFeatures(
    const char* ssid, const char* password, 
    const char* mqtt_server, int mqtt_port,
    int gmtOffset, int daylightOffset=-1,const char* ntp_server="pool.ntp.org"
    ) {
  addConnector(new SerialConnector());
  addFeature(new WiFiFeature(ssid,password));
  addConnector(new MQTTConnector(&client, mqtt_server, mqtt_port));
  addFeature(new NTPFeature(ntp_server,gmtOffset * 3600,daylightOffset * 3600));
  }

void BaseController::setupControls(const char* input ) {
  Serial.println(F("------+ Loading Controls +-------"));
  if( getConfig(controlsSetup, input, "/cont.json") ) {
    addFeature(new ControlManager(controlsSetup.as<JsonVariant>()));
  } else {
    Serial.println("!!! Problem loading Controls");
  }
  Serial.println(F("------+ Done Controls +-------"));
}

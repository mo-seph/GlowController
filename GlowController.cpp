#include "GlowController.h"
#include "GlowBehaviours.h"
#include "Arduino.h"
#include "GlowMQTT.h"
#include "GlowAlarms.h"



/*
Example JSON docs:

{"update":0, "data":{"r":0.4,"g":0.3,"b":0.2,"w":0.6}}
{"update":0, "data":{"r":0.9,"g":0.0,"b":0.0,"w":0.0}}
{"update":0, "data":{"r":0.0,"g":0.9,"b":0.0,"w":0.0}}


*/

GlowController::GlowController(GlowStrip* s, const char* id) :
  strip(s), id(id), behaviours(),  frameRate(25.0), doc(1200), defaultColor(0,0,0,0.2) {
    setTime(13,11,00);
    setDate(2021,05,29);
};

void GlowController::createBehaviours(JsonVariant d) {
  Serial.println(F("Creating behaviours from JSON: "));
  serializeJson(d,Serial);
  Serial.println("--");
  for(JsonVariant v : d.as<JsonArray>()) { createBehaviour(v); }
}

void GlowController::setBehaviour(int i, GlowBehaviour* b) {
  removeBehaviour(i);
  behaviours[i] = b;
  b->setActive(true);
}

void GlowController::removeBehaviour(int i) {
  if( behaviours[i] ) {
    behaviours[i]->setActive(false);
    delete behaviours[i];
  }
  behaviours[i] = NULL;
}

void GlowController::activateBehaviour(int i) {
  if( behaviours[i] ) behaviours[i]->setActive(true);
}

void GlowController::deActivateBehaviour(int i) {
  if( behaviours[i] ) behaviours[i]->setActive(false);
}

void GlowController::runBehaviours() {
  time.behavioursStart();

  strip->fillRGBW(0,0,0,0);
  int delta = time.delta();
  for( int i = 0; i < MAX_BEHAVIOURS; i++ ) {
    /* Update the behaviours */
    if(behaviours[i]) {
      //Serial.print(" Y:"); Serial.print(i);
      behaviours[i]->update(delta);
    }
    else {
      //Serial.print(" N:"); Serial.print(i);
    }
  }
  time.behavioursDone();


}

void GlowController::processInput(DynamicJsonDocument d) {
  Serial.println("Controller processing document");
  bool processed = false;
  if( d.containsKey("update") ) {
    Serial.print("Got update for behaviour "); Serial.println((int)d["update"]);
    int id = d["update"];
    Serial.println("Sent input to behaviour");
    updateBehaviour(id,d["data"]);
    sendState();
    processed = true;
  }
  /* Special one to store bacground colour in EEPROM */
  if( d.containsKey("update") && d["update"] == 0 ) {
      //Serial.println("Saving color");
      //serializeJson(doc["data"],Serial);
      //Serial.println();
      FRGBW newCol(d["data"]["r"], d["data"]["g"], d["data"]["b"], d["data"]["w"]);
      //strip.printColor(newCol);
      storeColor(newCol);
  }
  if( d.containsKey("activate") ) {
    activateBehaviour(d["activate"]);
    sendState();
    processed = true;
  }
  if( d.containsKey("deactivate")) {
    deActivateBehaviour(d["deactivate"]);
    sendState();
    processed = true;
  }
  if( d.containsKey("behaviours")) {
    Serial.println(F("Adding behaviours"));
    JsonArray behaviours = doc["behaviours"].as<JsonArray>();
    //serializeJson(behaviours,Serial);
    //Serial.println();
    createBehaviours(behaviours);
    Serial.println(F("Done loading behaviours"));
    sendState();
    processed = true;
  }
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
  if( d.containsKey("state")) {
    sendState();
    processed = true;
  }
  if( ! processed ) {
    Serial.print("Unknown document: ");
    serializeJson(d,Serial);
    Serial.println();
    Serial.println("-----");
  }


}

DynamicJsonDocument GlowController::createOutputState() {
  DynamicJsonDocument output(2048);
  for( int i = 0; i < MAX_BEHAVIOURS; i++ ) {
    if(behaviours[i]) {
      output[i]["active"] = behaviours[i]->isActive();
      output[i]["type"] = behaviours[i]->getType();
      output[i]["name"] = behaviours[i]->getName();
      output[i]["id"] = i;
      JsonVariant v = output[i].createNestedObject("data");
      //output["i"]["data"]["test"] = "hello";
      //behaviours[i]->addData(output["i"]["data"]);
      behaviours[i]->stateToJson(v);
    }
  }
  return output;
}

void GlowController::sendState() {
  Serial.println(F("Controller outputting state"));
  //Serial.flush();
  //Serial.send_now();
  DynamicJsonDocument output = createOutputState();
  JsonVariant opv = output.as<JsonVariant>();
  int l = connectors.size();
  for( int i = 0; i < l; i++ ) {
    connectors.get(i)->outputState(opv);
  }
  /*
  Serial.println("State on serial");
  serializeJson(output,Serial);
  Serial.println();
  Serial.println("State on MQTT");
  sendJsonMQTT(output);
  //Serial.flush();
  //Serial.send_now();
  Serial.println(F("State output done"));
  */
}

void GlowController::updateBehaviour(int id, JsonVariant d) {
  if( behaviours[id] != NULL ) {
    behaviours[id]->stateFromJson(d);
  }
}

void GlowController::createBehaviour(JsonVariant d) {
  Serial.println("Creating behaviour:");
  //serializeJson(d,Serial);
  int id = d["id"];
  const char* name = d["name"];
  const char* type = d["type"];
  Serial.print("Got id: "); Serial.println(id);
  Serial.print("Got name: "); Serial.println(name);
  Serial.print("Got type: "); Serial.println(type);
  Serial.flush();
  //delay(1000);

  Serial.println("About to make...");
  Serial.flush();
  //delay(200);
  //makeBehaviourFromType(type);
  GlowBehaviour* b = makeBehaviourFromType(type);
  //delay(400);
  Serial.println("Initialising behaviour");
  b->init(d);
  //delay(200);
  Serial.println("Setting behaviour");
  setBehaviour(id,b);
  //delay(200);
}

GlowBehaviour* GlowController::makeBehaviourFromType(const char* type) {
  Serial.print("Making behaviour from type: "); Serial.println(type);
  Serial.flush();
  //delay(300);
  if( strcmp(type,"Fill") == 0 ) {
    Serial.println("Making Fill");
    Serial.flush();
    //delay(300);
    Fill *f = new Fill(this);
    Serial.println("Made Fill");
    Serial.flush();
    //delay(300);
    return f;
  } else if(strcmp(type,"Watchdog") == 0) {
    Serial.println("Making Watchdog");
    return new Watchdog(this);
  } else if(strcmp(type,"PixelClock") == 0) {
    Serial.println("Making PixelClock");
    return new PixelClock(this);
  } else if(strcmp(type,"Glow") == 0 ) {
    Serial.println("Making Glow");
    return new Glow(this);
  } else if(strcmp(type,"Alarm") == 0 ) {
    Serial.println("Making Alarm");
    return new Alarms(this);
  }
  Serial.println("Making Unknown behaviour");
  return new GlowBehaviour(this,"Unknown");
}

void GlowController::loop() {
  time.loopStart();
  time.inputStart();
  updateFeatures();
  updateConnectors();
  time.inputDone();

  runBehaviours();

  time.showStart();
  strip->show();
  time.showDone();
  time.loopDone();
  time.printOutput();
  time.delayIfNeeded();
}

void GlowController::update() {
  if( Serial.available() > 4 ) {
    if( checkDeserialisation( deserializeJson(doc, Serial) ) )
      processInput(doc);
  }
}

void GlowController::update(const char* input) {
  if( checkDeserialisation( deserializeJson(doc, input) ) )
    processInput(doc);
}

void GlowController::update(byte* input, unsigned int length) {
  if( checkDeserialisation( deserializeJson(doc, input, length) ) )
    processInput(doc);
}


bool GlowController::checkDeserialisation(DeserializationError error) {
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

void setupWiFi(const char* ssid, const char* password) {
  Serial.println("Setting up WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");
  Serial.println(WiFi.localIP());
}

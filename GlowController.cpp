#include "GlowController.h"
#include "GlowBehaviours.h"
#include "Arduino.h"


/*
Example JSON docs:

{"update":0, "data":{"r":0.4,"g":0.3,"b":0.2,"w":0.6}}
{"update":0, "data":{"r":0.9,"g":0.0,"b":0.0,"w":0.0}}
{"update":0, "data":{"r":0.0,"g":0.9,"b":0.0,"w":0.0}}


*/



void GlowController::runBehaviours() {
  long start = millis();
  long delta = start - last_update;
  last_update = millis();

  if( blank ) {
    ////Serial.println("Blanking strip");
    //strip->fillRGBW(0,0,0,0);
    //Serial.println("Done");
  }
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
  long behaviour_time = millis() - last_update;
  /* Show the output */
  strip->show();

  long delay_time = (int)(1000 / frameRate) - delta;

  long show_time = millis() - last_update - behaviour_time;
  (void)show_time;
  #ifdef TIME_LOGGING
    Serial.print("Delta: ");Serial.print(delta);
    Serial.print(" Behaviour time: ");Serial.print(behaviour_time);
    Serial.print(" Show time: ");Serial.print(show_time);
    Serial.print(" Delay time: ");Serial.print(delay_time);
    Serial.println();
  #endif
  if( delay_time > 0 ) delay(delay_time);
}

void GlowController::processInput(DynamicJsonDocument d) {
  Serial.println("Controller processing document");
  if( d.containsKey("update") ) {
    Serial.print("Got update for behaviour "); Serial.println((int)d["update"]);
    int id = d["update"];
    Serial.println("Sent input to behaviour");
    updateBehaviour(id,d["data"]);
    sendState();
  }
  else if( d.containsKey("activate") ) {
    activateBehaviour(d["activate"]);
    sendState();
  }
  else if( d.containsKey("deactivate")) {
    deActivateBehaviour(d["deactivate"]);
    sendState();
  }
  else if( d.containsKey("add")) {
    Serial.println("Can't add behaviours yet...");
    sendState();
  }
  else if( d.containsKey("state")) {
    sendState();
  }
  else {
    Serial.print("Unknown document: ");
    serializeJson(d,Serial);
    Serial.println();
    Serial.println("-----");
  }
}

DynamicJsonDocument GlowController::outputState() {
  DynamicJsonDocument output(2048);
  Serial.println("Controller outputting state");
  for( int i = 0; i < MAX_BEHAVIOURS; i++ ) {
    if(behaviours[i]) {
      output[i]["active"] = behaviours[i]->isActive();
      output[i]["type"] = behaviours[i]->getName();
      output[i]["id"] = i;
      JsonVariant v = output[i].createNestedObject("data");
      //output["i"]["data"]["test"] = "hello";
      //behaviours[i]->addData(output["i"]["data"]);
      behaviours[i]->stateToJson(v);
    }
  }
  return output;
}

void GlowController::updateBehaviour(int id, JsonVariant d) {
  if( behaviours[id] != NULL ) {
    behaviours[id]->stateFromJson(d);
  }
}

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
  /* Show the output */
  strip->show();
}

void GlowController::processInput(DynamicJsonDocument d) {
  if( d.containsKey("update") ) {
    int id = d["update"];
    updateBehaviour(id,d["data"]);
  }
}

void GlowController::updateBehaviour(int id, DynamicJsonDocument d) {
  if( behaviours[id] != NULL ) {
    behaviours[id]->updateParameters(d);
  }
}

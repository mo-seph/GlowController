#include "GlowBehaviours.h"

GlowBehaviour::GlowBehaviour(GlowController *c,const char *type) :
  controller(c), strip(c->getStrip()), type(type) { }

bool Fill::loadInitialColor(FRGBW& data) {
    Serial.print("Loading color for base fill from Prefs::");Serial.println(saveKey);
    if( controller->loadColor(data,saveKey) ) {
      Serial.println("Found color!");
      data.toSerial();
      fadeIn(data,1.5);
      return true;
    }
    else {
      Serial.print("No colour found in "); Serial.println(saveKey);
    }
  
    return false;
  }

bool Fill::saveInitialColor(FRGBW& data) {
    if( save ) {
      Serial.print("Saving color for base fill to Prefs::");Serial.println(saveKey);
      data.toSerial();
      controller->storeColor(data,saveKey);
      return true;
    }
    return false;
  }

PixelClock::PixelClock(GlowController* s ) : GlowBehaviour(s,"PixelClock") {}
void PixelClock::doUpdate(long millis) {
  strip->startPixels(start,scale);
  strip->addPixel(delimiterCol);
  CurrentTime *t = controller->getTime();
  int h = t->hour;
  int m = t->minute / 5;
  int s = t->second / 5;

  for( int i = 0; i < 24; i++ ) {
    strip->addPixel(h > i ? hoursCol : backgroundCol );
  }
  strip->addPixel(delimiterCol);
  for( int i = 0; i < 12; i++ ) {
    strip->addPixel(m > i ? minutesCol : backgroundCol );
  }
  strip->addPixel(delimiterCol);
  for( int i = 0; i < 12; i++ ) {
    strip->addPixel(s > i ? secondsCol : backgroundCol );
  }
  strip->addPixel(delimiterCol);
};

void PixelClock::stateToJson(JsonVariant d) {
  d["start"] = start;
  d["scale"] = scale;
}

void PixelClock::stateFromJson(JsonVariant d) {
  Serial.println("Updating pixelclock");
  serializeJson(d,Serial);
  if( d.containsKey("start")) start = d["start"];
  if( d.containsKey("scale")) scale = d["scale"];
}

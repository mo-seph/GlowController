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
  strip->startPixels(start);
  strip->addPixel(delimiterCol);
  CurrentTime *t = controller->getTime();
  float h = t->hour/24.0;
  float m = t->minute /60.0;
  float s = t->second / 60.0;

  int hours_start = start + 1;
  strip->drawBar(hours_start, hours_start+hours_length,hoursCol,h,backgroundCol);
  strip->addPixel(delimiterCol);
  int mins_start = hours_start + hours_length + 1;
  strip->drawBar(mins_start, mins_start + mins_length,minutesCol,m,backgroundCol);
  strip->addPixel(delimiterCol);
  int secs_start = mins_start + mins_length + 1;
  if( secs_length > 0 ) {
    strip->drawBar(secs_start, secs_start + secs_length,secondsCol,s,backgroundCol);
    strip->addPixel(delimiterCol);
  }


  /*
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
  */
};

void PixelClock::stateToJson(JsonVariant d) {
  d["start"] = start;
  //d["scale"] = scale;
}

void PixelClock::stateFromJson(JsonVariant d) {
  Serial.println("Updating pixelclock");
  serializeJson(d,Serial);
  if( d.containsKey("start")) start = d["start"];
  //if( d.containsKey("scale")) scale = d["scale"];
}

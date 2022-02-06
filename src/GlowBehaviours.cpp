#include "GlowBehaviours.h"

GlowBehaviour::GlowBehaviour(GlowController *c,const char *type) :
  controller(c), strip(c->getStrip()), type(type) { }


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

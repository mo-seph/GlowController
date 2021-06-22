#ifndef GlowHelpers_h
#define GlowHelpers_h
#include "Arduino.h"
#include "GlowStrip.h"
//#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <GlowController.h>



class GlowBall {

protected:
  AreaRGBW glow;
  float rate;
  float direction = 1;
  bool active;

public:
  //GlowBall() : GlowBall(AreaARGBW(FRGBW(0,0,0,0),0,0), 0, 1, false) { };
  GlowBall(AreaRGBW g, float r, int dir, bool act=false) :
    glow(g), rate(r), direction(dir), active(act){ };

  void updateDynamics(long millis) {
    if( rate < 0 ) {
      rate = -rate;
      direction = -1;
    }
    if(! active ) { return; }
    glow.centre = glow.centre + rate * millis * 0.001 * direction;
    if( glow.centre > 1.0 ) {
      glow.centre = 1.0;
      direction = -1;
    }
    if( glow.centre < 0.0 ) {
      glow.centre = 0.0;
      direction = 1;
    }
  }

  void show(GlowStrip *s) {
    s->glowRGBWRel(glow,true);
  }

  void toJson(JsonVariant d) {
    glow.toJson(d);
    d["rate"] = rate;
  }

  void fromJson(JsonVariant d) {
    glow.fromJson(d);
    if( d.containsKey("rate")) rate = d["rate"];
  }
};

class Breath  {
protected:
  float level;

  GlowStrip* strip;
  FRGBW color;
  float rate;
  float max;
  float min;
  float delta;
  int count;
  int startPixel;
  int endPixel;
  bool active;

public:
  Breath(GlowStrip* strip, FRGBW color, float rate, float max=1.0, float min = 0.0) :
    strip(strip), color(color), rate(rate), max(max), min(min), active(false) {
      level = min;
      delta = rate;
      startPixel = 0;
      endPixel = strip->length()-1;
    }

  void setRange(int start, int end) {
    startPixel = start;
    endPixel = end;
  }
  void setRange(float start, float end) {
    setRange(strip->positionToPixels(start),strip->positionToPixels(end));
  }

  void setCycles(int number) {
    count = number;
    active = true;
  }

  void activate() {
    count = -1;
    active = true;
  }

  void doUpdate(long millis) {
    if(!active) {return;}
    level += ((float)millis * 0.001 ) * delta;
    if( level >= max ) {
      level = max;
      delta = -rate;
    }
    if( level <= min ) {
      level = min;
      delta = rate;
      if( count > 0 ) {count--; }
      if( count == 0 ) { active = false; }
    }
    //Serial.print("Level: "); Serial.print(level);
    //Serial.print(", Delta: "); Serial.println(delta);
    //strip->fillRGBW(strip->interpolateRGBW(FRGBW(0,0,0,0),color,level),startPixel,endPixel);
    strip->fillRGBW(strip->interpolateRGBW(FRGBW(0,0,0,0),color,level),startPixel,endPixel);
  }

};


#endif

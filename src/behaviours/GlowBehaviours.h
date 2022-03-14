#ifndef GlowBehaviour_h
#define GlowBehaviour_h
#include "Arduino.h"
#include "strip/GlowStrip.h"
//#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <GlowController.h>
#include "strip/GlowHelpers.h"
#include "ColorStructures.h"
//#include <TimeLib.h>

#include "TimeStruct.h"


class GlowController;


class GlowBehaviour {
public:
  GlowBehaviour(GlowController *c,const char *type);

  virtual void init(JsonVariant d) {
    if( d.containsKey("name")) setName( d["name"] );
    if(d.containsKey("data")) stateFromJson(d["data"]);
    if(d.containsKey("active")) active = d["active"];
    else active = true;
  };

  virtual ~GlowBehaviour() {};


  virtual void stateFromJson(JsonVariant d) {};
  virtual void stateToJson(JsonVariant d) {}


  virtual void update(long millis) { if(active) doUpdate(millis); };
  virtual void doUpdate(long millis) {};

  virtual const char* getName() {return name;};
  virtual void setName(const char* n) {
    strcpy(name,n);
    Serial.print(F("Set name to: ")); Serial.println(name);
  };


  void setActive(bool act) {
    if( ! active && act ) { active = true; }
    else if( active && !act ) { active = false; }
  }
  bool isActive() { return active; };
  virtual const char* getType() {return type;};

  /* Quick function to make it easy to create output at certain intervals.
   * Call it with the interval, if it's been long enough, it will update
   * the last output time, and return true; otherwise, returns false
   */
  bool timedOutput(long interval) {
    if(millis() > lastOutput + interval ) {
      lastOutput = millis();
      return true;
    }
    else {
      return false;
    }
  }



protected:
  GlowController *controller;
  GlowStrip *strip;
  //char name[50];
  char name[50] = "NoName";
  bool active;
  const char* type;
  long lastOutput = 0;
};


class Fill : public GlowBehaviour {
public:
  Fill(GlowController* s) : GlowBehaviour(s,"Fill") {}
  /*
  Fill(GlowStrip* s, FRGBW col) :
    Fill(s), currentColor(col), targetColor(col) { }
    */

  void doUpdate(long updateTime) {
    long delta = millis() - interpStart;
    float proportion = (float)((delta)/interpTime) * 0.001;
    if( proportion <= 1.0 ) {
      currentColor = strip->interpolateRGBW(startColor,targetColor,proportion);
    } else {

    }
    //strip->printColor(currentColor);
    //Serial.println();
    strip->fillRGBW(currentColor);
  }


  void setInterpTime(float t) { nextInterpTime = t;}

  void setColor(FRGBW c ) {
    startInterp();
    targetColor = c;
    currentColor = c;
  }

  void fadeIn(FRGBW target, float time) {
    float tmpInterp = interpTime;
    currentColor = FRGBW(0,0,0,0);
    interpTime = time;
    targetColor = target;
    startInterp();
    setInterpTime(tmpInterp);

  }

  void stateFromJson(JsonVariant d) {
    if( d.containsKey("time")) nextInterpTime = d["time"];
    // Start from current colour
    FRGBW tmpCol = targetColor;
    // Update it from the JSON coming in
    tmpCol.fromJson(d);
    // If the HSV has been updated, then use that as truth, and convert
    // into the RGBW colour
    if( hsvCol.fromJson(d) ) {
      Serial.println("Got a HSV update...");
      hsvCol.toRGBW(tmpCol);
      tmpCol.toSerial();
    }
    setColor(tmpCol);
  }

  void stateToJson(JsonVariant d) {
    targetColor.toJson(d);
    hsvCol.toJson(d);
    d["time"] = (nextInterpTime > 0) ? nextInterpTime : interpTime;
  }

protected:
  FRGBW currentColor;
  FRGBW startColor;
  FRGBW targetColor;
  FHSV hsvCol;
  float interpTime =0;
  float nextInterpTime = -1;
  long interpStart;

  void startInterp() {
    startColor = currentColor;
    if( nextInterpTime > 0 ) {
      interpTime = nextInterpTime;
      Serial.print("Setting interp time: "); Serial.println(interpTime);
      nextInterpTime = -1;
    }
    interpStart = millis();
  }
};



class Watchdog : public GlowBehaviour {
public:
  Watchdog(GlowController* s) : GlowBehaviour(s, "Watchdog") {
    length = 10;
    factor = 10;
    counterColor = FRGBW(1,0,0,0);
  }
  /*
  Watchdog(GlowStrip* s, int l=10, int factor=10, float bri = 0.2) :
    Watchdog(s), length(l),factor(factor),brightness(bri) {
      counterColor = FRGBW(0,0,0,bri);
    };
    */

  void doUpdate(long updateTime) {
    current = (current + 1 ) % (factor*(length-2));
    long t2 = updateStored(updateTime);
    strip->addRGBW(current/factor + 2,counterColor);
    strip->setRGBW(0,timeToColor(updateTime));
    strip->setRGBW(1,timeToColor(t2));
  }

  long updateStored(long t) {
    previousCounter = (previousCounter + 1) % 30;
    previousTimes[previousCounter] = t;
    for(int i = 0; i < 30; i++ ) t = max(t,previousTimes[i]);
    return t;
  }

  FRGBW timeToColor(long updateTime) {
    if( updateTime < 10 ) return FRGBW(0,1,1,0);
    if( updateTime < 30 ) return FRGBW(0,1,0,0);
    if( updateTime < 100 ) {
      float factor = (float)(updateTime-30)/70.0;
      return FRGBW(factor,(1-factor),0,0);
    }
    return FRGBW(1,0,1,0);
  }

  virtual void stateFromJson(JsonVariant d) {
    if( d.containsKey("length")) length = d["length"];
    if( d.containsKey("factor")) factor = d["factor"];
  }

  virtual void stateToJson(JsonVariant d) {
    d["length"] = length;
    d["factor"] = factor;
  }

protected:
  int length;
  int current;
  int factor;
  float brightness;
  FRGBW counterColor;
  long previousTimes[30];
  int previousCounter;
};


/*
class FillHSV : public Behaviour {
  virtual const char* getName() {return "FillHSV";};
  void init(const uint8_t* buffer, size_t size) {
    strip->fill(strip->gamma32(strip->ColorHSV(buffer[0]*255,buffer[1],buffer[2])));
    strip->show();
  }
};
*/




class Glow : public GlowBehaviour {

public:
  Glow(GlowController* s, AreaRGBW g, float rate=0) :
    GlowBehaviour(s,"Glow"), ball(g,rate,1,true) { }
  Glow(GlowController* s) : Glow(s,AreaRGBW(FRGBW(0,0,0,1),0.1,0.01)) {};

  void doUpdate(long millis) {
    ball.updateDynamics(millis);
    ball.show(strip);
  }

  virtual void stateFromJson(JsonVariant d) {
    ball.fromJson(d);
  }

  virtual void stateToJson(JsonVariant d) {
    ball.toJson(d);
  }
protected:
  GlowBall ball;
};

class PixelClock : public GlowBehaviour {
public:
  PixelClock(GlowController* s );
  void doUpdate(long millis);
  virtual void stateToJson(JsonVariant d);
  virtual void stateFromJson(JsonVariant d);

protected:
  int start = 10;
  int scale = 1;
  FRGBW backgroundCol=FRGBW(0,0,0,0);
  FRGBW delimiterCol=FRGBW(0,0,1,0);
  FRGBW hoursCol=FRGBW(1,0,0,0);
  FRGBW minutesCol=FRGBW(0,1,0,0);
  FRGBW secondsCol=FRGBW(0,1,0,0);

};

/*
#define MAX_BALLS 16
class GlowBalls : public GlowBehaviour {

public:
  Glow(GlowController* s ) : BallRunner(s,"GlowBalls") { }

  void update(long millis) {
    for( int i = 0; i < MAX_BALLS; i++ ) updateBall(balls[i], millis);
  }

  void updateBall(GlowBall* ball, long millis) {
    if(! ball->active ) { return; }
    ball->glow.centre = ball->glow.centre + ball->rate * millis * 0.001 * ball->direction;
    if( ball->glow.centre > 1.0 ) {
      ball->glow.centre = 1.0;
      ball->direction = -1;
    }
    if( ball->glow.centre < 0.0 ) {
      ball->glow.centre = 0.0;
      ball->direction = 1;
    }
    strip->glowRGBWRel(ball->glow,true);
  }

protected:
  GlowBall balls[MAX_BALLS]
};
*/

/*

class Breathe : public GlowBehaviour {
protected:
  float level;

  FRGBW color;
  float rate;
  float max;
  float min;
  float delta;

  ;

public:
  Breathe(GlowController* s, FRGBW col, float r, float mx=1.0, float mn = 0.0) :
    GlowBehaviour(s,"Breathe"), color(col), rate(r), max(mx), min(mn) {
      level = min;
      delta = rate;
    }
  void doUpdate(long millis) {
    level += ((float)millis * 0.001 ) * delta;
    if( level >= max ) {
      level = max;
      delta = -rate;
    }
    if( level <= min ) {
      level = min;
      delta = rate;
    }
    Serial.print("Level: "); Serial.print(level);
    Serial.print(", Delta: "); Serial.println(delta);
    strip->fillRGBW(strip->interpolateRGBW(FRGBW(0,0,0,0),color,level));
  }

};

*/

#endif

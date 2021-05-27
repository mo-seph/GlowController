#ifndef GlowBehaviour_h
#define GlowBehaviour_h
#include "Arduino.h"
#include "GlowStrip.h"
//#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>


class GlowBehaviour {
public:
  GlowBehaviour(GlowStrip *s,const char* nme) : strip(s), name(nme) {};
  //void set_strip(GlowStrip *s){ strip = s;}
  virtual void init(JsonVariant d) { init(); };
  virtual void init() {};


  virtual void stateFromJson(JsonVariant d) {};
  virtual void stateToJson(JsonVariant d) {}

  virtual void update(long millis) {
    if(active) doUpdate(millis);
  };
  virtual void doUpdate(long millis) {};

  virtual const char* getName() {return name;};

  void setActive(bool act) {
    if( ! active && act ) {
      Serial.print("Behaviour beginning: "); Serial.println(getName());
      active = true;
    }
    else if( active && !act ) {
      Serial.print("Behaviour ending: "); Serial.println(getName());
      active = false;
    }
  }
  bool isActive() { return active; };




protected:
  GlowStrip *strip;
  const char* name;
  bool active;

};


class Fill : public GlowBehaviour {
public:
  Fill(GlowStrip* s, FRGBW col) : GlowBehaviour(s,"Fill"), currentColor(col) {
    targetColor = currentColor;
  }
  void doUpdate(long updateTime) {
    long delta = millis() - interpStart;
    float proportion = (float)((delta)/interpTime) * 0.001;
    if( proportion <= 1.0 ) {
      currentColor = strip->interpolateRGBW(startColor,targetColor,proportion);
    } else {

    }
    strip->fillRGBW(currentColor);
  }


  void setInterpTime(float t) { nextInterpTime = t;};
  void setColor(FRGBW c ) {
    Serial.print("Setting color: "); strip->printColor(c);
    Serial.println();
    startInterp();
    targetColor = c;
  };

  void stateFromJson(JsonVariant d) {
    if( d.containsKey("time")) nextInterpTime = d["time"];
    FRGBW tmpCol = targetColor;
    tmpCol.fromJson(d);
    setColor(tmpCol);
  };

  void stateToJson(JsonVariant d) {
    targetColor.toJson(d);
    d["time"] = (nextInterpTime > 0) ? nextInterpTime : interpTime;
  }

protected:
  FRGBW currentColor;
  FRGBW startColor;
  FRGBW targetColor;
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
  };
};

class Watchdog : public GlowBehaviour {
public:
  Watchdog(GlowStrip* s, int l=10, int factor=10, float bri = 0.2) :
    GlowBehaviour(s,"Watchdog"), length(l),factor(factor),brightness(bri) {
      counterColor = FRGBW(0,0,0,bri);
    };

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
    if( updateTime < 10 ) return FRGBW(0,1,0,0);
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
  int previousTimes[30];
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



class Glow : public GlowBehaviour {

public:
  Glow(GlowStrip* s) : Glow(s,AreaRGBW(FRGBW(0,0,0,1),0.1,0.01)) {};
  Glow(GlowStrip* s, AreaRGBW g, float rate=0) :
    GlowBehaviour(s,"Glow"), ball(g,rate,1,true) { }

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


/*
#define MAX_BALLS 16
class GlowBalls : public GlowBehaviour {

public:
  Glow(GlowStrip* s ) : BallRunner(s,"GlowBalls") { }

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
  Breathe(GlowStrip* s, FRGBW col, float r, float mx=1.0, float mn = 0.0) :
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

#endif

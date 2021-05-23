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
  virtual void init(JsonDocument d) { init(); };
  virtual void init() {};
  virtual void updateParameters(DynamicJsonDocument d) {};
  virtual void update(long millis) {};
  virtual const char* getName() {return name;};
  //Utility...
  void start() {Serial.print("Behaviour beginning: "); Serial.println(getName());};

  void stop() {Serial.print("Behaviour ending: "); Serial.println(getName());};

protected:
  GlowStrip *strip;
  const char* name;

};


class Fill : public GlowBehaviour {
public:
  Fill(GlowStrip* s, FRGBW col) : GlowBehaviour(s,"Fill"), currentColor(col) {
    targetColor = currentColor;
  }
  void update(long updateTime) {
    long delta = millis() - interpStart;
    float proportion = (float)((delta)/interpTime) * 0.001;
    if( proportion <= 1.0 ) {
      currentColor = strip->interpolateRGBW(startColor,targetColor,proportion);
    } else {

    }
    strip->fillRGBW(currentColor);
  }

  void updateParameters(DynamicJsonDocument d) {
    if( d.containsKey("time")) {
      nextInterpTime = (d["time"]);
    }
    FRGBW tmpCol;
    tmpCol = targetColor;
    if( d.containsKey("r")) { tmpCol.r = d["r"]; }
    if( d.containsKey("g")) tmpCol.g = d["g"];
    if( d.containsKey("b")) tmpCol.b = d["b"];
    if( d.containsKey("w")) tmpCol.w = d["w"];
    setColor(tmpCol);
  };

  void setInterpTime(float t) { nextInterpTime = t;};
  void setColor(FRGBW c ) {
    Serial.print("Setting color: "); strip->printColor(c);
    Serial.println();
    startInterp();
    targetColor = c;
  };
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

  void update(long updateTime) {
    current = (current + 1 ) % (factor*length);
    strip->addRGBW(current/factor,counterColor);
  }

protected:
  int length;
  int current;
  int factor;
  float brightness;
  FRGBW counterColor;
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

/*

struct GlowBall {
  AreaRGBW glow;
  float rate;
  float direction = 1;
  bool active;
  GlowBall() {
    glow = AreaARGBW(FRGBW(0,0,0,0),0,0);
    rate = 0;
    direction = 1;
    active = false;
  };
  GlowBall(AreaARGB g, float r, int dir, bool act=false) {
    glow = g;
    rate = r;
    direction = dir;
    active = act;
  };
};


#define MAX_BALLS=16

class BallRunner : public GlowBehaviour {

public:
  BallRunner(GlowStrip *s,const char* nme) : GlowBehaviour(s,nme) {};
protected:
  void updateBallDynamics(GlowBall* ball, long millis) {
    if( ball->rate < 0 ) {
      ball->rate = -ball->rate;
      ball->direction = -1;
    }
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


};

class Glow : public BallRunner {

public:
  Glow(GlowStrip* s) : Glow(s,AreaARGBW(FRGBW(0,0,0,0),0,0)) {};
  Glow(GlowStrip* s, AreaRGBW g, float r=0) : BallRunner(s,"Glow"), ball(g,r,1,true) { }
  void update(long millis) {
    updateBallDynamics(&ball,millis);
  }
protected:
  GlowBall ball;
};


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
  void update(long millis) {
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

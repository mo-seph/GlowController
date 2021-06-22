#ifndef GlowStrip_h
#define GlowStrip_h

#include "Arduino.h"
#include <ArduinoJson.h>



/**
* This is a float based data container for LED strips
* Absolute waste of memory, but makes some things easier
*/

struct FRGBW  {
  union {
    struct {
      union {
        float g;
        float green;
      };
      union {
        float r;
        float red;
      };
      union {
        float b;
        float blue;
      };
      union {
        float w;
        float white;
      };
    };
    float raw[4];
  };

  FRGBW(){}

  FRGBW(float rd, float grn, float blu, float wht){
    r = rd;
    g = grn;
    b = blu;
    w = wht;
  }

  void fromJson(JsonVariant d) {
    if( d.containsKey("r")) r = d["r"];
    if( d.containsKey("g")) g = d["g"];
    if( d.containsKey("b")) b = d["b"];
    if( d.containsKey("w")) w = d["w"];
  };

  void toJson(JsonVariant d) {
    d["r"] = r;
    d["g"] = g;
    d["b"] = b;
    d["w"] = w;
  }
};







struct PointRGBW {
  FRGBW color;
  float position;
  PointRGBW( FRGBW c, float p ) {
    color = c;
    position = p;
  }
};

struct PointARGBW {
  FRGBW color;
  int position;
  PointARGBW( FRGBW c, int p ) {
    color = c;
    position = p;
  }
};

struct AreaRGBW {
  FRGBW color;
  float centre;
  float width;
  AreaRGBW( FRGBW col, float cent, float w)
  {
    color = col;
    centre = cent;
    width = w;
  }

  void fromJson(JsonVariant d) {
    color.fromJson(d);
    if( d.containsKey("centre")) centre = d["centre"];
    if( d.containsKey("width")) width = d["width"];
  };

  void toJson(JsonVariant d) {
    color.toJson(d);
    d["centre"] = centre;
    d["width"] = width;
  };
};

struct AreaARGBW {
  FRGBW color;
  int centre;
  int width;
  /*
  AreaRGBW( FRGBW col, int cent, int w)
  {
  color = col;
  centre = cent;
  width = w;
}
*/
};


/*
FRGBW interpolateFRGBW(FRGBW start_col, FRGBW end_col, float amount ) {
return FRGBW(
start_col.r + (end_col.r - start_col.r)*amount,
start_col.g + (end_col.g - start_col.g)*amount,
start_col.b + (end_col.b - start_col.b)*amount,
start_col.w + (end_col.w - start_col.w)*amount
);
}
*/

class GlowStrip {
public:
  GlowStrip(int n, FRGBW* c) : numLeds(n), current(c){ }

  /* Main interface */
  /* Sets the full pixel colors */
  void setRGBW(int num, float r, float g, float b, float w);
  void setRGBW(int num, FRGBW val);
  /* Sets RGB, leaves the white alone */
  void setRGB(int num, float r, float g, float b);
  /* Sets W, leaves the rgb alone */
  void setW(int num, float w);

  void addRGBW(int num, FRGBW val);

  /* Helper functions */
  virtual void fillRGBW(float r, float g, float b, float w, int from=0, int to=0) {
    if( to == 0 ) to = numLeds;
    for( int i = from; i < to; i++ ) setRGBW(i,r,g,b,w);
  }
  virtual void fillRGBW(FRGBW val, int from=0, int to=0) {
    if( to == 0 ) to = numLeds;
    for( int i = from; i < to; i++ ) setRGBW(i,val);
  }
  virtual void fillRGB(float r, float g, float b, int from=0, int to=0) {
    if( to == 0 ) to = numLeds;
    for( int i = from; i < to; i++ ) setRGB(i,r,g,b);
  }
  virtual void fillW(float w, int from=0, int to=0) {
    if( to == 0 ) to = numLeds;
    for( int i = from; i < to; i++ ) setW(i,w);
  }

  void printColor(FRGBW c) {
    Serial.print(" R: "); Serial.print(c.r);
    Serial.print(" G: "); Serial.print(c.g);
    Serial.print(" B: "); Serial.print(c.b);
    Serial.print(" W: "); Serial.print(c.w);
  }

  void glowRGBWRel(AreaRGBW a, bool additive = false);

  void spatialInterpolation(PointARGBW from, PointARGBW to);

  inline FRGBW interpolateRGBW(FRGBW start_col, FRGBW end_col, float amount) {
    return FRGBW(
      start_col.r + (end_col.r - start_col.r)*amount,
      start_col.g + (end_col.g - start_col.g)*amount,
      start_col.b + (end_col.b - start_col.b)*amount,
      start_col.w + (end_col.w - start_col.w)*amount
    );
  }

  int length() {return numLeds;};

  int positionToPixels( float position ) {
    return (int)(position * numLeds );
  }

  /* Main functions to rewrite for an implementation */

  /* Uses the underlying controller to output the whole strip */
  virtual void show();
  virtual void init() {};

  /* Starts a block of pixels. Each call to addPixel will
  set the colour and move to the next one. Scale determines
  how many LEDs are used for each pixel */
  void startPixels(int position,int scale = 1) {
    currentPosition = position;
    pixelScale = scale;
  }
  void addPixel(FRGBW color,bool additive=false) {
    for( int i = 0; i < pixelScale; i++ ) {
      if( additive )  addRGBW(currentPosition, color);
      else setRGBW(currentPosition, color);
      currentPosition++;
    }
  }


protected:
  int numLeds;
  FRGBW* current;
  float brightness = 1.0;
  int currentPosition = 0;
  int pixelScale = 1;
};

/*
class GlowControllerStrip : public GlowStrip {
GlowControllerStrip(int numLeds, int numColors,)
}
*/
#endif

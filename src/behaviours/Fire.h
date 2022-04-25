#ifndef GlowFire_h
#define GlowFire_h
#include "Arduino.h"
#include "strip/GlowStrip.h"
#include <ArduinoJson.h>
#include <GlowController.h>
#include "behaviours/GlowBehaviours.h"
#include <LinkedList.h>



class FireBehaviour : public GlowBehaviour {
public:
  FireBehaviour(GlowController* s) : GlowBehaviour(s,"Fire") {
    colors.addPoint(0.0,FRGBW(0,0,0,0));
    //colors.addPoint(0.01,FRGBW(0,0,0.1,0));
    //colors.addPoint(0.02,FRGBW(0,0,0.0,0));
    colors.addPoint(0.2,FRGBW(0.1,0.025,0,0));
    colors.addPoint(0.3,FRGBW(0.2,0.05,0,0.0));
    colors.addPoint(0.95,FRGBW(0.75,0.25,0.0,0.05));
    colors.addPoint(1.0,FRGBW(1,0.65,0.0,0.15));
  }
  void doUpdate(long updateTime) {
    //colors.print();
    current += speed;

    int startPix = strip->positionToPixels(start);
    int endPix = strip->positionToPixels(end);
    for( int i = startPix; i < endPix; i++ ) {
       strip->setRGBW(i, colors.getColor(pixelIntensity(i))) ;
    }
  }

  float pixelIntensity(int pixel) {
    float pos = strip->pixelsToPosition(pixel);
    float v =
      (0.5+(sin( 1.0 * (ripple*pos) + 1.0 * current) * 0.5 )) * 0.2 +
      (0.5+(sin( 2.0 * (ripple*pos) + 1.0 * current) * 0.5 )) * 0.1 +
      (0.5+(sin( 0.3 * (ripple*pos) + 0.2 * current) * 0.5 )) * 0.3 +
      (0.5+(sin( -1.2 * (ripple*pos) + 1.2 * current) * 0.5 )) * 0.2 +
      (0.5+(sin( -2.1 * (ripple*pos) + 1.8 * current) * 0.5 )) * 0.2 +
      0;
    return clamp(v*intensity);
    //float half = mapF(0.5,0,1,start,end);
    //if( pos < half ) { return mapF(pos,start,half,0,1); }
    //return mapF(pos,half,end,1,0);
  }



  void stateFromJson(JsonVariant d) {
    if( d.containsKey("start")) start = d["start"];
    if( d.containsKey("end")) start = d["end"];
    if( d.containsKey("intensity")) intensity = d["intensity"];
    if( d.containsKey("speed")) speed = d["speed"];
    if( d.containsKey("ripple")) ripple = d["ripple"];
  }

  void stateToJson(JsonVariant d) {
    d["start"] = start;
    d["end"] = end;
    d["intensity"] = intensity;
    d["speed"] = speed;
    d["ripple"] = ripple;
  }

protected:
  float start = 0.0;
  float end = 1.0;
  float intensity = 0.5;
  float speed = 0.01;
  float ripple = 100.0;
  float current = 0;
  InterpolationTable colors;
};

#endif

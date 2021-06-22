#ifndef FastLEDGlowStrip_h
#define FastLEDGlowStrip_h

#include <FastLED.h>
#include "GlowStrip.h"
#include "FastLED_RGBW.h"
#include "Arduino.h"

class FastLEDGlowStrip : public GlowStrip {
public:
  FastLEDGlowStrip(int n, CRGBW* l, FRGBW* c) : GlowStrip(n,c) {
      leds = l;

  };

  void show() {

    long start_t = millis();
    for( int i = 0; i < numLeds; i++ ) {
      leds[i] = toCRGBW(current[i]);
    }
    long convert_t = millis();
    FastLED.show();
    long show_t = millis();
    #ifdef TIME_LOGGING
      Serial.print("Time taken... Convert: ");Serial.print(convert_t-start_t);
      Serial.print(", Show: ");Serial.print(show_t-convert_t);
      Serial.print(", Total Display: ");Serial.println(show_t-start_t);
    #else
      // Avoid unused variable warnings
      (void)start_t;(void)convert_t;(void)show_t;
    #endif
  };

  CRGBW toCRGBW(FRGBW c) {
    int ri = fromFloat(c.r);
    int gi = fromFloat(c.g);
    int bi = fromFloat(c.b);
    int wi = fromFloat(c.w);
    return CRGBW(ri, gi, bi, wi );
  }

  inline int fromFloat(float v) {
    return min(max(v * 255.0, 0.0), 255.0);
  }

protected:
  CRGBW* leds;
};


#endif

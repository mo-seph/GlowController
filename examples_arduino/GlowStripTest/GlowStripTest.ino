


/* FastLED RGBW Example Sketch
 *
 * Example sketch using FastLED for RGBW strips (SK6812). Includes
 * color wipes and rainbow pattern.
 *
 * Written by David Madison
 * http://partsnotincluded.com
*/

#include "FastLED.h"

#include "FastLEDGlowStrip.h"
#include "FastLED_RGBW.h"

#define NUM_LEDS 21
#define DATA_PIN 2


int x = 4;

FRGBW current[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 255;

FastLEDGlowStrip strip(NUM_LEDS,leds, current);


void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setBrightness(brightness);
  strip.fillRGBW(0,0,0,0);
  strip.show();
}

void loop(){

  for( float i = 0.0; i < 1; i+=0.02) {
    strip.fillRGBW(0,0,1,0);
    strip.glowRGBWRel(i,0.1,FRGBW(1,0,0,0), true);
    //strip.glowRGBWRel(i,0.1,FRGBW(1,0,0,0));
    strip.show();
    delay(200);
  }


  /*
  strip.setW(0,1.0); strip.show(); delay(200);
  strip.setRGB(0,1,0,0); strip.show(); delay(200);
  strip.setW(0,0.0); strip.show(); delay(200);
  strip.setRGB(0,0,1,0); strip.show(); delay(200);
  */
  /*
  for( float i = 0.0; i < 1; i+=0.01) {
    double x = 0;
    strip.fill(modf(i+0.4f,&x),modf(i+0.3,&x),modf(i+0.9,&x),i);
    strip.show();
    delay(10);
  }*/


}

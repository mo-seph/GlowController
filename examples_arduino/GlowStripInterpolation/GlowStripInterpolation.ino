


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

PointARGBW a( FRGBW(1,0,0,0), 0 );
PointARGBW b( FRGBW(0,0,1,1), NUM_LEDS );

float apos = 0;
float adir = -1.0;
float aspeed = 0.01;

float bpos = 0.5;
float bdir = -1.0;
float bspeed = 0.006;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setBrightness(brightness);
  strip.fillRGBW(0,0,0,0);
  strip.show();
}

void loop(){

  strip.fillRGBW(0,0,0,0);
  apos += adir * aspeed;
  if( apos > 1.0 ) { apos = 1; adir = -1; }
  if( apos < 0.0 ) { apos = 0; adir = 1; }

  bpos += bdir * bspeed;
  if( bpos > 1.0 ) { bpos = 1; bdir = -1; }
  if( bpos < 0.0 ) { bpos = 0; bdir = 1; }

  a.position = (int)(apos * NUM_LEDS);
  b.position = (int)(bpos * NUM_LEDS);


  strip.spatialInterpolation(a,b);
  strip.show();
  delay(20);

}

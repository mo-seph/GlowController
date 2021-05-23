


/* FastLED RGBW Example Sketch
 *
 * Example sketch using FastLED for RGBW strips (SK6812). Includes
 * color wipes and rainbow pattern.
 *
 * Written by David Madison
 * http://partsnotincluded.com
*/

#include "FastLED.h"

#include "GlowStrip.h"

#include "FastLEDGlowStrip.h"
#include "GlowBehaviours.h"
#include "FastLED_RGBW.h"

#define NUM_LEDS 1521
#define DATA_PIN 2


FRGBW current[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 255;

FastLEDGlowStrip strip(NUM_LEDS,leds, current);

float width = 49.0 / NUM_LEDS;
float rate = 140.0 / NUM_LEDS;

Glow g1(&strip,AreaRGBW(FRGBW(1,0,0,0),0.0,width),rate*1.1);
Glow g2(&strip,AreaRGBW(FRGBW(0,1,0,0),0.01,width),rate*0.96);
Glow g3(&strip,AreaRGBW(FRGBW(0,0,1,0),0.02,width),rate*0.8);
Glow g4(&strip,AreaRGBW(FRGBW(0,0,0,1),0.03,width),rate);

long lastUpdate = millis();

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setBrightness(brightness);
  strip.show();
}

void loop(){
  delay(10);
  long t = millis();
  long delta = t - lastUpdate;
  lastUpdate = t;
  strip.fillRGBW(0,0,0,0);
  g1.update(delta);
  g2.update(delta);
  g3.update(delta);
  g4.update(delta);
  strip.show();
  long update_t = millis() - t;
  Serial.print("Update time: ");Serial.print(update_t);Serial.print(" FPS: ");Serial.println((int)(1000.0/update_t));


}

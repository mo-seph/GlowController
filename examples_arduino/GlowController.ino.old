


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
#include "GlowController.h"
#include "ArduinoJson.h"
#include "FastLED_RGBW.h"

#define NUM_LEDS 1200
#define DATA_PIN 2


FRGBW current[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 255;

FastLEDGlowStrip strip(NUM_LEDS,leds, current);

float width = 49.0 / NUM_LEDS;
float rate = 140.0 / NUM_LEDS;

Fill base(&strip,FRGBW(0,0,0,0.0));
Watchdog watchdog(&strip,20);

GlowController glowControl(&strip,true);

float r = 0;

long lastUpdate = millis();

char json[] =
      "{\"update\":0,\"data\":{\"r\":0.5,\"g\":0.1,\"b\":0.3,\"w\":0.7}}";
FRGBW defaultColor(0,0,0,0.5);
DynamicJsonDocument doc(200);

void setup() {
  //while(!Serial) {}
  delay(200);
  Serial.println("Starting out...");

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setBrightness(brightness);
  strip.show();

  glowControl.setBehaviour(0,&base);
  glowControl.setBehaviour(16,&watchdog);


  
  FRGBW startupColor = glowControl.loadColor(defaultColor);
  base.setInterpTime(16);
  base.setColor(startupColor);
  delay(100);
  base.setInterpTime(.2);
}

void loop(){
  if( Serial.available() > 4 ) {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, Serial);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if( doc.containsKey("update")) {
      Serial.print("Got update for behaviour "); Serial.println((int)doc["update"]);
      glowControl.processInput(doc);
      Serial.println("Sent input to behaviour");
      
      if(doc["update"] == 0 ) {
        Serial.println("Saving color");
        serializeJson(doc["data"],Serial);
        
        FRGBW newCol(doc["data"]["r"], doc["data"]["g"], doc["data"]["b"], doc["data"]["w"]);
        strip.printColor(newCol);
        glowControl.storeColor(newCol);
      }
      
    } else {
      Serial.print("Got unknown document");      
    }
  }
  
  long t = millis();
  glowControl.runBehaviours();
  //long update_t = millis() - t;
  //Serial.print("Update time: ");Serial.print(update_t);Serial.print(" FPS: ");Serial.println((int)(1000.0/update_t));
  delay(10);

}

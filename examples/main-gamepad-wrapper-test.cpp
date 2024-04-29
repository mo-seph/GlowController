
#include <Arduino.h>
#include "controls/Gamepad.h"

#include <Adafruit_NeoPixel.h>
#define PIN        PIN_NEOPIXEL
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

GamepadWrapper gc = GamepadWrapper();

void setup() {
    Serial.begin(119200);
    while( !Serial ) {delay(10);}
    gc.init();
    Serial.println("Starting up");
  pixels.begin();
}

void loop() {
    gc.update();
    if( gc.A_TRIGGERED ) Serial.println("A");
    if( gc.B_TRIGGERED ) Serial.println("B");
    if( gc.X_TRIGGERED ) Serial.println("X");
    if( gc.Y_TRIGGERED ) Serial.println("Y");
    if( gc.HAS_MOVED ) {Serial.print("Moved: x="); Serial.print(gc.current_x); Serial.print(", y="); Serial.println(gc.current_y);}
    if( gc.X_DISPLACED ) {Serial.print("Xdisp: x="); Serial.println(gc.current_x);
    }
    if( gc.Y_DISPLACED ) {Serial.print("Ydisp: y="); Serial.println(gc.current_y);
    }

    if( gc.X_DISPLACED || gc.Y_DISPLACED ) {
        pixels.setPixelColor(0, pixels.Color(gc.last_x /4, gc.last_y/4, 0));
    } else pixels.setPixelColor(0, pixels.Color(0,0,0));
    pixels.show();
    delay(10);
}
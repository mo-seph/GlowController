#ifndef LED_STATUS_FEATURE_H
#define LED_STATUS_FEATURE_H

#include "features/GlowFeature.h"
#include "GlowController.h"
#include <Arduino.h>

#include <Adafruit_NeoPixel.h>



class LEDStatusFeature : public Feature  {
public:
  LEDStatusFeature(int pin=PIN_NEOPIXEL, int order = NEO_GRB + NEO_KHZ800) : Feature() {
    //pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
    pixels = Adafruit_NeoPixel(1, pin, order);
    pixels.begin();
  };

  // Additive model for colour to overlay indications
  virtual void update() {
    /*
    r = 0;
    g = 0;
    b = 0;
    // Check in with WiFi - does solid Blue / Green
    if( MQTTConnector::static_mqtt->isConnected() ) { g = 1; }
    */
    //pixels.setPixelColor(0, pixels.ColorHSV(r, g, b));
    bool wifi = WiFi.status() != WL_CONNECTED;
    bool mqtt = wifi && GLOBAL_MQTT_CONNECTOR != NULL && GLOBAL_MQTT_CONNECTOR->isConnected();

    // Start with low red glow
    pixels.setPixelColor(0, pixels.ColorHSV(0, 255, 50));
    if( wifi && mqtt ) pixels.setPixelColor(0, pixels.ColorHSV(120, 255, 50));
    // Should we blink?
    if( ( millis() % blinkRate ) < blinkCycle ) {
      // If not connected to wifi, blink red
      if( !wifi ) { pixels.setPixelColor(0, pixels.ColorHSV(0, 255, 255)); }
      else { pixels.setPixelColor(0, pixels.ColorHSV(180, 255, 255)); }
    }
    pixels.show();
  }

protected:
  float r;
  float g;
  float b;
  int blinkRate = 1000;
  int blinkCycle = 300;
  Adafruit_NeoPixel pixels;
};

#endif

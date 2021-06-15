#ifndef GLOW_FEATURE_H
#define GLOW_FEATURE_H

#include "GlowController.h"
#include "ArduinoJson.h"

class GlowController;

class Feature {
public:
  Feature() {};
  virtual void update();
  void setController(GlowController *c) {
    controller = c;
  };

protected:
  GlowController *controller;
};

class Connector {
public:
  Connector() {};
  virtual void update(DynamicJsonDocument doc) {};
  virtual void outputState(JsonVariant v) {};
  virtual void ping(JsonVariant v) {};

  virtual void setController(GlowController *c) {
    controller = c;
  };

  bool checkDeserialisation(DeserializationError error) {
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    } else {
      Serial.println("Got JSON input");
      return true;
    }
  }

protected:
  GlowController *controller;
};



#endif

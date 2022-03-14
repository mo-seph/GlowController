#ifndef GLOW_CONNECTOR_H
#define GLOW_CONNECTOR_H

#include "connectors/GlowConnectors.h"
#include "ArduinoJson.h"

class BaseController;


class Connector {
public:
  Connector() {};
  virtual void update() {};
  virtual void outputState(JsonVariant v) {};
  virtual void ping(JsonVariant v) {};

  virtual void setController(BaseController *c) {
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
  BaseController *controller;
};



#endif

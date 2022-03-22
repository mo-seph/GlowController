#ifndef GLOW_FEATURE_H
#define GLOW_FEATURE_H

//#include "GlowController.h"
#include "ArduinoJson.h"

class BaseController;

class Feature {
public:
  Feature() {};
  virtual void update();
  virtual void setController(BaseController *c) {
    controller = c;
  };

protected:
  BaseController *controller;
};

#endif

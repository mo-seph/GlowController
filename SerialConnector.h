#ifndef SERIAL_CONNECTOR_H
#define SERIAL_CONNECTOR_H

#include "GlowInterfaces.h"
#include "ArduinoJson.h"

class SerialConnector : public Connector  {
public:
  virtual void update(DynamicJsonDocument doc) {
    //Serial.println("Looking for serial input");
    if( Serial.available() > 4 ) {
      if( checkDeserialisation( deserializeJson(doc, Serial) ) )
        Serial.println("Found input, sending to controller");
        controller->processInput(doc);
    }
  }

  virtual void outputState(JsonVariant v) {
    Serial.println(F("Serial output of state..."));
    serializeJson(v,Serial);
    Serial.println();
  }
};

#endif

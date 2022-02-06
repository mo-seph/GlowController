#ifndef SERIAL_CONNECTOR_H
#define SERIAL_CONNECTOR_H

#include "GlowInterfaces.h"
#include "ArduinoJson.h"

class SerialConnector : public Connector  {
public:
  virtual void update() {
    //Serial.println("Looking for serial input");
    if( Serial.available() > 4 ) {
      DynamicJsonDocument inputDoc(3000);
      if( checkDeserialisation( deserializeJson(inputDoc, Serial) ) )
        Serial.println("Found input, sending to controller");
        controller->processInput(inputDoc.as<JsonVariant>());
    }
  }

  virtual void outputState(JsonVariant v) {
    Serial.println(F("Serial output of state..."));
    serializeJson(v,Serial);
    Serial.println();
  }

  virtual void ping(JsonVariant doc) {
    Serial.println("Serial Pinging");
    serializeJson(doc, Serial);
    Serial.println();
  }
};

#endif

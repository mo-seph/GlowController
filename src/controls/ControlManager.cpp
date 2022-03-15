#include "ControlManager.h"
#include "BaseController.h"
#include <ArduinoJson.h>

void Controllers::sendUpdate(JsonVariant v ) {
     Serial.println("Sending control message");
        serializeJson(v, Serial);
        controller->processInput(v);
}
#include "ControlActions.h"



void ControlAction::setController(BaseController* b) {
    controller = b;
}
void InternalGlowAction::send() {
    Serial.println("Sending document internally: ");
    serializeJson(doc,Serial);
    controller->processInput(doc.as<JsonVariant>());
}
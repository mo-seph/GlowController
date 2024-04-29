#include "ControlActions.h"


void ControlAction::toggle() { toggled = !toggled; trigger(toggled);}
void ControlAction::triggerMult(float in) { trigger(in * multiplier);}
ControlAction::ControlAction(JsonVariant config) {
    if( config.containsKey("multiplier")) {
        multiplier = config["multiplier"];
    } else multiplier = 1.0;
}

void ControlAction::setController(BaseController* b) {
    controller = b;
}
void InternalGlowAction::send() {
    Serial.println("Sending document internally: ");
    serializeJson(doc,Serial); Serial.println();
    controller->processInput(doc.as<JsonVariant>());
}
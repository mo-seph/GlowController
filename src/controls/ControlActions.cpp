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

void ToggleButton::action() {
        Serial.print("Setting toggle: ");
        Serial.println(value);
        actions->trigger(value);
    }

void ValueButton::action() {
    Serial.print("Setting toggle: ");
    Serial.println(value);
    actions->trigger(value);
}

void CenteredContinuous::action() {
    Serial.print("Setting continous: ");
    Serial.println(range.getValue() );
    actions->trigger(range.getValue());
}

ControlActionList::ControlActionList(JsonVariant v, char* key, bool warn) {
    addActions(v, key, warn);
}
void ControlActionList::addActions(JsonVariant v, char* key, bool warn) {
    if( v.containsKey(key)) {
        if( v[key].is<JsonArray>()) {
            for( int i = 0; i < v[key].as<JsonArray>().size(); i++ ) 
                addAction(createAction(v[key][i]));
        } else {
            addAction(createAction(v[key]));
        }
    } else {
        if(warn) {Serial.print("Couldn't make actions for key: "); Serial.println(key);}
    }

}

ControlAction* ControlActionList::createAction(JsonVariant v ) {
    if( v["type"] == "internal") {return new InternalGlowAction(v);}
    if( v["type"] == "external") {return new ExternalGlowAction(v);}
    Serial.println("Unknown action spec: ");
    serializeJson(v,Serial);
    return NULL;
}

void ControlActionList::setController(BaseController* b ) {
    controller = b;
    for( int i=0; i < actions.size(); i++ ) actions.get(i)->setController(b);
}
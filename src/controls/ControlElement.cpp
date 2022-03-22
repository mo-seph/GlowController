#include "controls/ControlActions.h"
#include "BaseController.h"

void ControlElement::setController(BaseController* b ) {
    controller = b;
    for( int i=0; i < actions.size(); i++ ) actions.get(i)->setController(b);
}
ControlAction* ControlElement::createAction(JsonVariant v ) {
    if( v["type"] == "internal") {return new InternalGlowAction(v);}
    if( v["type"] == "external") {return new ExternalGlowAction(v);}
    Serial.println("Unknown action spec: ");
    serializeJson(v,Serial);
    return NULL;
}

void ControlElement::trigger(float in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}
void ControlElement::trigger(bool in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}
void ControlElement::trigger(int in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}
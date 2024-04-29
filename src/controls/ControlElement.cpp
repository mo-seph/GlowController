#include "controls/ControlActions.h"
#include "BaseController.h"
#include "Gamepad.h"

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


GamepadElement::GamepadElement(JsonVariant config) : ControlElement(config) {
    if(config.containsKey("up_pressed")) up_pressed = createAction(config["up_pressed"]);
    if(config.containsKey("down_pressed")) down_pressed = createAction(config["down_pressed"]);
    if(config.containsKey("left_pressed")) left_pressed = createAction(config["left_pressed"]);
    if(config.containsKey("right_pressed")) right_pressed = createAction(config["right_pressed"]);
    if(config.containsKey("x_displaced")) {
        Serial.println("Got action for x_displaced: ");
        serializeJson(config["x_displaced"], Serial);
        x_displaced = createAction(config["x_displaced"]);
    }
    if(config.containsKey("y_displaced")) y_displaced = createAction(config["y_displaced"]);
    Serial.print("Setting up Gamepad...");
    gc.init();
    Serial.println("Done!");
}

bool GamepadElement::update(int i ) {
    gc.update();
    //gc.printStatus();
    if( gc.UP_TRIGGERED ) {
        Serial.println("UP!");
        if( up_pressed ) up_pressed->toggle();
    }
    if( gc.DOWN_TRIGGERED ) {
        Serial.println("DOWN Pressed!");
        if( down_pressed ) down_pressed->toggle();
    }
    if( gc.LEFT_TRIGGERED ) {
        Serial.println("LEFT Pressed!");
        if( left_pressed ) left_pressed->toggle();
    }
    if( gc.RIGHT_TRIGGERED ) {
        Serial.println("RIGHT Pressed!");
        if( right_pressed ) right_pressed->toggle();
    }
    if( gc.X_DISPLACED ) {
        Serial.print("X_DISPLACED...: "); Serial.println(gc.current_x);
        if( x_displaced ) x_displaced->triggerMult(gc.current_x);
    }
    if( gc.Y_DISPLACED ) {
        if( y_displaced ) y_displaced->triggerMult(gc.current_y);
    }
    return false;
}
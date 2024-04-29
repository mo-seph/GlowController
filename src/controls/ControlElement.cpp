#include "controls/ControlActions.h"
#include "BaseController.h"
#include "Gamepad.h"

ControlElement::ControlElement(JsonVariant config) : hasChanged(false)
{
    Serial.print("Creating control ");
    Serial.println(config["type"].as<String>());
    serializeJson(config,Serial);
    actions = new ControlActionList(config,"actions",false);
}

void ControlElement::setController(BaseController* b ) {
    controller = b;
    actions->setController(b);
}
ControlAction* ControlElement::createAction(JsonVariant v ) {
    if( v["type"] == "internal") {return new InternalGlowAction(v);}
    if( v["type"] == "external") {return new ExternalGlowAction(v);}
    Serial.println("Unknown action spec: ");
    serializeJson(v,Serial);
    return NULL;
}

//void ControlElement::trigger(float in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}
//void ControlElement::trigger(bool in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}
//void ControlElement::trigger(int in) { for( int i = 0; i < actions.size(); i++ ) actions.get(i)->trigger(in);}


GamepadElement::GamepadElement(JsonVariant config) : ControlElement(config) {
    Serial.print("Setting up Gamepad...");
    up_pressed = new ControlActionList(config,"up_pressed");
    down_pressed = new ControlActionList(config,"down_pressed");
    left_pressed = new ControlActionList(config,"left_pressed");
    right_pressed = new ControlActionList(config,"right_pressed");
    x_displaced = new ControlActionList(config,"x_displaced");
    y_displaced = new ControlActionList(config,"y_displaced");
    gc.init();
    Serial.println("Done!");
}

bool GamepadElement::update(int i ) {
    gc.update();
    //gc.printStatus();
    if( gc.UP_TRIGGERED ) {
        Serial.println("UP!");
        up_pressed->toggle();
    }
    if( gc.DOWN_TRIGGERED ) {
        Serial.println("DOWN Pressed!");
        down_pressed->toggle();
    }
    if( gc.LEFT_TRIGGERED ) {
        Serial.println("LEFT Pressed!");
        left_pressed->toggle();
    }
    if( gc.RIGHT_TRIGGERED ) {
        Serial.println("RIGHT Pressed!");
        right_pressed->toggle();
    }
    if( millis() > last_update + JOYSTICK_TIME ) {
        last_update = millis();
        if( gc.X_DISPLACED ) {
            Serial.print("X_DISPLACED...: "); Serial.println(gc.current_x);
            x_displaced->triggerMult(gc.current_x);
        }
        if( gc.Y_DISPLACED ) {
            Serial.print("Y_DISPLACED...: "); Serial.println(gc.current_y);
            y_displaced->triggerMult(gc.current_y);
        }
    }
    return false;
}
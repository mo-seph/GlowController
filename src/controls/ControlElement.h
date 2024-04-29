#ifndef CONTROL_ELEMENT_H
#define CONTROL_ELEMENT_H

#include "features/GlowFeature.h"
#include "ControlActions.h"
#include "BaseController.h"
#include <ArduinoJson.h>
#include <Bounce2.h>
#include "Utils.h"
#include "ControlHelpers.h"
#include "LinkedList.h"
#include "Gamepad.h"

class BaseController;
class ControlAction;

class ControlElement
{
public:
    ControlElement(JsonVariant config) : hasChanged(false)
    {
        Serial.println("Creating control");
        serializeJson(config,Serial);
        if(config.containsKey("actions")) {
            for( int i = 0; i < config["actions"].as<JsonArray>().size(); i++ ) {
                ControlAction* a = createAction(config["actions"][i]);
                if( a != NULL ) actions.add(a);
            }
        }
    }
    virtual bool update(int i)
    {
        read();
        if (hasChanged)
        {
            hasChanged = false;
            action();
            return true;
        } else {
            return false;
        }
    }
    /* Should all be generic I think! */
    virtual void trigger(float in);
    virtual void trigger(bool in);
    virtual void trigger(int in);

    virtual void action() { Serial.println("Base class action = shouldn't happen..."); }
    virtual void read() { Serial.println("Base class read - shouldn't happen..."); }
    virtual void setController(BaseController* b );
    static ControlAction* createAction(JsonVariant v );
      
protected:
    BaseController* controller;
    bool hasChanged;
    LinkedList<ControlAction*> actions;
};




class ToggleButton : public ControlElement {
public:
    ToggleButton(JsonVariant v) : ControlElement(v), value(false) {
      pin = v["pin"].as<int>();
      pinMode(pin,INPUT_PULLUP);
      bounce.attach(pin,INPUT_PULLUP);
    }

  virtual void read()
    {
        bounce.update();
        if (bounce.fallingEdge() ) {
            hasChanged = true;
            value = !value;
        }
    }

    virtual void action() {
        Serial.print("Setting toggle: ");
        Serial.println(value);
        trigger(value);
    }

protected:
    int pin;
    Bounce bounce = Bounce();
    bool value;
};

class ValueButton : public ControlElement {
public:
    ValueButton(JsonVariant v) : ControlElement(v), value(0.0) {
      pin = v["pin"].as<int>();
      pinMode(pin,INPUT_PULLUP);
      bounce.attach(pin,INPUT_PULLUP);
      value = v["value"];
    }

  virtual void read()
    {
        bounce.update();
        if (bounce.fallingEdge() ) {
            hasChanged = true;
        }
    }

    virtual void action() {
        Serial.print("Setting toggle: ");
        Serial.println(value);
        trigger(value);
    }

protected:
    int pin;
    Bounce bounce = Bounce();
    float value;
};

/*
Example JSON
 {
      "type":"cknob",
      "pin":34,
      "max":2,
      "min":-2,
      "minIn":0.25,
      "maxIn":0.75,
      "centerIn":0.45,
      "deadZone":0.05,
      "update":100,
      "behaviour_id":0,
      "path":["data","h+"]
    },
    */
class CenteredContinuous : public ControlElement {
public:
    CenteredContinuous(JsonVariant v) : ControlElement(v) {
      pin = v["pin"].as<int>();
      if(v.containsKey("update")) rate = v["update"];
      if(v.containsKey("min")) range.minOut = v["min"];
      if(v.containsKey("center")) range.centerOut = v["center"];
      if(v.containsKey("max")) range.maxOut = v["max"];
      if(v.containsKey("minIn")) range.minIn = v["minIn"];
      if(v.containsKey("centerIn")) range.centerIn = v["centerIn"];
      if(v.containsKey("maxIn")) range.maxIn = v["maxIn"];
      if(v.containsKey("deadZone")) range.deadZone = v["deadZone"];
    }

  virtual void read()
    {
        long val = analogRead(pin);
        float fval = (float)val/4096.0;
        long now = millis();
        // At first, just calibrate the centre
        if( now < startupDelay ) {
            range.updateCenter(fval,true);
        }
        // Then check every so often whether we are away from the centre
        else {
            range.update(fval);
            if( range.isActive() && now > (nextUpdate ) ) {
                nextUpdate = now + rate;
                hasChanged = true;
                Serial.print("Updating continuous knob! ");
                Serial.print(fval);
                Serial.print(" --> "); Serial.println(range.getValue());
                Serial.print("Waiting for ms: "); Serial.println(rate);
            }
        }
    }

    virtual void action() {
        Serial.print("Setting continous: ");
        Serial.println(range.getValue() );
        trigger(range.getValue());
    }

protected:
    int pin;
    CenteringRange range;
    long startupDelay = 3000;
    long nextUpdate = 3000; // Don't do anything for the first 3 seconds...
    long rate = 300;
};

/* Example JSON:
  {
      "type":"gamepad",
      "deadzone":0.05,
      "update":200,
      "x_displaced": 
        {
          "type":"external",
          "behaviour_id":0,
          "multiplier":0.1,
          "target":"study_shelf",
          "path":["data","s+"]
        } 
    }
*/

class GamepadElement : public ControlElement {
    GamepadWrapper gc = GamepadWrapper();
    int x_button_index = 0;
    int y_button_index = 0;
    int a_button_index = 0;
    int b_button_index = 0;

public:
    GamepadElement(JsonVariant config);
    bool update(int i );

protected:
    ControlAction* x_displaced = NULL;
    ControlAction* y_displaced = NULL;
    ControlAction* up_pressed = NULL;
    ControlAction* down_pressed = NULL;
    ControlAction* left_pressed = NULL;
    ControlAction* right_pressed = NULL;
};

#endif
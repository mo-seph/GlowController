#ifndef CONTROL_ELEMENT_H
#define CONTROL_ELEMENT_H

#include "features/GlowFeature.h"
#include "BaseController.h"
#include <ArduinoJson.h>
#include <Bounce2.h>

class BaseController;

class ControlElement
{
public:
    ControlElement(JsonVariant config) : doc(200), hasChanged(false)
    {
        Serial.println("Creating control");
        serializeJson(config,Serial);
        JsonArray path = config["path"].as<JsonArray>();
        doc["update"] = config["id"];
        target = doc.as<JsonVariant>();//.createNestedObject("data");
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i].is<char *>()) {
                Serial.print("Adding path: ");
                Serial.println(path[i].as<const char *>());
                target = target.createNestedObject(path[i].as<const char *>());
            }
            serializeJson(doc, Serial);
            Serial.println();
        }
    }
    virtual bool update(int i)
    {
        read();
        if (hasChanged)
        {
            hasChanged = false;
            fillIn();
            Serial.println("Sending...");
            //controllers->sendUpdate(doc.as<JsonVariant>());
            return true;
        } else {
            return false;
        }
    }

    virtual void fillIn() { Serial.println("Base class fill = shouldn't happen..."); }
    virtual void read() { Serial.println("Base class read - shouldn't happen..."); }
    JsonVariant getCurrent() { return doc.as<JsonVariant>(); }

protected:
    DynamicJsonDocument doc;
    JsonVariant target;
    bool hasChanged;
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
        if (bounce.fallingEdge() )
        {
            hasChanged = true;
            value = !value;
        }
    }

    virtual void fillIn() {
        Serial.print("Setting toggle: ");
        Serial.println(value);
        target.set(value);
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
        if (bounce.fallingEdge() )
        {
            hasChanged = true;
        }
    }

    virtual void fillIn() {
        Serial.print("Setting toggle: ");
        Serial.println(value);
        target.set(value);
    }

protected:
    int pin;
    Bounce bounce = Bounce();
    float value;
};

class ContinuousKnob : public ControlElement {
public:
    ContinuousKnob(JsonVariant v) : ControlElement(v) {
      pin = v["pin"].as<int>();
      if(v.containsKey("min")) min = v["min"];
      if(v.containsKey("max")) max = v["max"];
    }

  virtual void read()
    {
        long val = analogRead(pin);
        float fval = (float)val/4096.0;
        //Serial.println(fval);
        long now = millis();
        if ((abs(fval-0.5) > deadzone) && (now > (lastUpdate + rate) ) )
        {
            Serial.print("Updating continuous knob! ");
            Serial.println(fval);
            hasChanged = true;
            lastUpdate = now + rate;
            value = map(fval,0.0,1.0,min,max);
            Serial.println(" --> "); Serial.println(value);
        }
    }

    virtual void fillIn() {
        Serial.print("Setting continous: ");
        Serial.println(value);
        target.set(value);
    }

protected:
    int pin;
    float deadzone = 0.2;
    float value = 0.0;
    float min = 0.0;
    float max = 1.0;
    long lastUpdate = 3000; // Don't do anything for the first 3 seconds...
    long rate = 300;
};


#endif
#ifndef CONTROL_MANAGER_H
#define CONTROL_MANAGER_H

#include "features/GlowFeature.h"
#include "BaseController.h"
#include <ArduinoJson.h>
#include <Bounce2.h>
#include "ControElement.h"

class BaseController;

/* Assume setup is something like
{

"controls" :
    [
        {
            "type":"knob",
            "pin":5,
            "path":[1, "rate"]
        },
        {
            "type":"toggle",
            "pin":4,
            "path":[1, "active"]
        }
    ]
}
*/

const int MAX_CONTROLS = 16;

// Add a public addControl method to allow other features going in
class ControlManager : public Feature
{
public:
    ControlManager(JsonVariant setup) : Feature()
    {
        JsonArray controlSpec = setup["controls"].as<JsonArray>();
        for (int i = 0; i < MAX_CONTROLS; i++)
        {
            if (i < controlSpec.size())
            {
                controls[i] = createControl(controlSpec[i]);
                //controls[i]->setControllers(this);
            }
            else
                controls[i] = NULL;
        }
    };

    ControlElement *createControl(JsonVariant spec)
    {
        if (spec["type"] == "toggle")
        {
            ControlElement * t = new ToggleButton(spec);
            return t;
        }
        if (spec["type"] == "value")
        {
            ControlElement * t = new ValueButton(spec);
            return t;
        }
        if (spec["type"] == "cknob")
        {
            ControlElement * t = new ContinuousKnob(spec);
            return t;
        }
        else
        {
            Serial.println("Unknown control setup:");
            serializeJson(spec, Serial);
        }
        return NULL;
    }

    virtual void update()
    {
        for (int i = 0; i < MAX_CONTROLS; i++)
        {
            if (controls[i]) { 
                if( controls[i]->update(1) ) { sendUpdate(controls[i]->getCurrent() );} 
                //if( controls[i]->update() ) ;
            }
        }
    }
    void sendUpdate(JsonVariant v );

protected:
    ControlElement *controls[MAX_CONTROLS];
};



#endif
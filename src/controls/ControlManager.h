#ifndef CONTROL_MANAGER_H
#define CONTROL_MANAGER_H

#include "features/GlowFeature.h"
#include "BaseController.h"
#include <ArduinoJson.h>
#include <Bounce2.h>
#include "ControlElement.h"
#include "LinkedList.h"

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

// Add a public addControl method to allow other features going in
class ControlManager : public Feature
{
public:
    ControlManager(JsonVariant setup) : Feature()
    {
        JsonArray controlSpec = setup["controls"].as<JsonArray>();
        for (int i = 0; i < controlSpec.size(); i++) {
            ControlElement* c = createControl(controlSpec[i]);
            if( c != NULL ) controls.add( c );
        }
    };

    ControlElement *createControl(JsonVariant spec)
    {
        if (spec["type"] == "toggle") { return new ToggleButton(spec); }
        if (spec["type"] == "value") { return new ValueButton(spec); }
        if (spec["type"] == "cknob") { return new CenteredContinuous(spec); }
        if (spec["type"] == "gamepad") { return new GamepadElement(spec); }
        else
        {
            Serial.println("Unknown control setup:");
            serializeJson(spec, Serial);
        }
        return NULL;
    }

    virtual void update()
    {
        int s = controls.size();
        for (int i = 0; i < s; i++) {
            controls.get(i)->update(1);
        }
    }

    virtual void setController(BaseController* b ) {
        Feature::setController(b);
        for (int i = 0; i < controls.size(); i++) {
            controls.get(i)->setController(b); 
        }
    }

protected:
    LinkedList<ControlElement*> controls;
};



#endif
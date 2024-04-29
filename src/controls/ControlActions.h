#ifndef CONTROL_ACTIONS_H
#define CONTROL_ACTIONS_H

#include <ArduinoJson.h>
#include "BaseController.h"
#include "connectors/GlowMQTT.h"

class ControlAction
{
public:
    ControlAction(JsonVariant config);
    virtual void trigger(float in) {}
    virtual void triggerMult(float in);
    virtual void trigger(bool in) {}
    virtual void trigger(int in) {}
    virtual void toggle();
    void setController(BaseController* b);
protected:
    BaseController* controller;
    bool toggled = false;
    float multiplier = 1;
};



class GlowJSONAction : public ControlAction {
public:
    GlowJSONAction(JsonVariant config) : ControlAction(config), doc(200) {
        JsonArray path = config["path"].as<JsonArray>();
        doc["update"] = config["behaviour_id"];
        target = doc.as<JsonVariant>();
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i].is<const char *>()) {
                Serial.print("Adding path: ");
                Serial.println(path[i].as<const char *>());
                target = target.createNestedObject(path[i].as<const char *>());
            }
            serializeJson(doc, Serial);
            Serial.println();
        }

    }
    //Send the JSON packet
    virtual void send() {};
    virtual void trigger(float in) { target.set(in); send(); }
    virtual void trigger(bool in) { target.set(in); send(); }
    virtual void trigger(int in) { target.set(in); send(); }
protected:
    DynamicJsonDocument doc;
    JsonVariant target;
};


class InternalGlowAction : public GlowJSONAction {
public:
    InternalGlowAction(JsonVariant config) : GlowJSONAction(config) {
        Serial.println("Setting up internal control action...");
    }
    virtual void send();
};
class ExternalGlowAction : public GlowJSONAction {
public:
    ExternalGlowAction(JsonVariant config) : GlowJSONAction(config) {
        Serial.println("Setting up external control action...");
        serializeJson(config,Serial);Serial.println();
        target = String("leds/") + config["target"].as<const char*>() + "/commands";
        Serial.print("Starting action pointing at: "); Serial.println(target);
    }
    virtual void send() {
        Serial.println("Sending document externally to: ");
        serializeJson(doc,Serial); Serial.println();
        MQTTConnector::static_mqtt->sendJSON(target.c_str(),doc.as<JsonVariant>());
    }
protected:
    String target;
};

#endif
#ifndef CONTROL_ACTIONS_H
#define CONTROL_ACTIONS_H

#include <ArduinoJson.h>
//#include "BaseController.h"
#include "connectors/GlowMQTT.h"
#include "LinkedList.h"

class BaseController;

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
        GLOBAL_MQTT_CONNECTOR->sendJSON(target.c_str(),doc.as<JsonVariant>());
    }
protected:
    String target;
};

class ControlActionList {
public:
    ControlActionList(JsonVariant v, char* key, bool warn=false);
    void addActions(JsonVariant v, char* key, bool warn=false);

    ControlAction* createAction(JsonVariant v );
    void addAction(ControlAction* a) {
        if( a != NULL ) {
            actions.add(a);
            if( controller != NULL ) { a->setController(controller); }
        }
    }
    void setController(BaseController* b );
    void trigger(float in) { for( int i=0; i < actions.size(); i++ ) actions.get(i)->trigger(in); }
    void triggerMult(float in) { for( int i=0; i < actions.size(); i++ ) actions.get(i)->triggerMult(in); }
    void trigger(bool in) { for( int i=0; i < actions.size(); i++ ) actions.get(i)->trigger(in); }
    void trigger(int in)  { for( int i=0; i < actions.size(); i++ ) actions.get(i)->trigger(in); }
    void toggle() { for( int i=0; i < actions.size(); i++ ) actions.get(i)->toggle(); }

protected:
    LinkedList<ControlAction*> actions;
    BaseController* controller = NULL;
};

#endif
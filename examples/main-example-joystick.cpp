
#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
//#include "Arduino.h"
#include "ControlDefaults.h"
#include "GlowController.h"

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1


BaseController glowControl(strname(DEVICE_ID),strname(DEVICE_NAME));

/*
static const char controlsJSON[] PROGMEM = ( R"(
{
  "controls":
  [
    {
      "type":"knob",
      "id":1,
      "pin":5,
      "path":[1,"rate"]
    },
    {
      "type":"toggle",
      "id":1,
      "pin":4,
      "path":["hello","active"]
    }
  ]
})");
*/

static const char controlsJSON[] PROGMEM = ( R"(
{
  "controls":
  [
    {
      "type":"cknob",
      "pin":34,
      "max":0.05,
      "min":-0.05,
      "deadzone":0.05,
      "minIn":0.23,
      "maxIn":0.75,
      "update":200,
      "actions":[ 
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living",
          "path":["data","s+"]
        }, 
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living_beam",
          "path":["data","s+"]
        } 
      ]
    },
    {
      "type":"cknob",
      "pin":39,
      "max":10,
      "min":-10,
      "deadzone":0.05,
      "minIn":0.23,
      "maxIn":0.75,
      "update":200,
      "actions":[ 
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living",
          "path":["data","h+"]
        },
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living_beam",
          "path":["data","h+"]
        }
      ]
    },
      {
      "type":"cknob",
      "pin":36,
      "max":0.05,
      "min":-0.05,
      "deadzone":0.05,
      "minIn":0.23,
      "maxIn":0.75,
      "update":200,
      "actions":[ 
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living",
          "path":["data","v+"]
        },
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living_beam",
          "path":["data","v+"]
        }
      ]
    },
    {
      "type":"toggle",
      "pin":26,
      "actions":[
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living",
          "path":["active"]
        },
        {
          "type":"external",
          "behaviour_id":0,
          "target":"living_beam",
          "path":["active"]
        }
      ]
    }
  ]
})");

void setup() {
    glowControl.initialise();
    glowControl.setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    glowControl.setupControls(controlsJSON);
}

void loop() {
    glowControl.loop();
}
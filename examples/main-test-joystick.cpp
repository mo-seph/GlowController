
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
      "max":2,
      "min":-2,
      "deadzone":0.05,
      "minIn":0.23,
      "maxIn":0.75,
      "update":100,
      "actions":[ 
        {
          "type":"internal",
          "behaviour_id":0,
          "path":["data","s+"]
        },
        {
          "type":"external",
          "behaviour_id":0,
          "target":"study_test",
          "path":["data","s+"]
      } ]
    },
    {
      "type":"cknob",
      "pin":39,
      "max":2,
      "min":-2,
      "deadzone":0.05,
      "minIn":0.23,
      "maxIn":0.75,
      "update":100,
      "actions":[ 
        {
          "type":"internal",
          "behaviour_id":0,
          "path":["data","h+"]
        },
        {
          "type":"external",
          "behaviour_id":0,
          "target":"study_test",
          "path":["data","h+"]
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
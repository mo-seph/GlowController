
#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
//#include "Arduino.h"
#include "ControlDefaults.h"
#include "GlowController.h"

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1


BaseController glowControl(strname(DEVICE_ID),strname(DEVICE_NAME));

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

void setup() {
    glowControl.initialise();
    glowControl.setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    glowControl.setupControls(controlsJSON);
}

void loop() {
    glowControl.loop();
}
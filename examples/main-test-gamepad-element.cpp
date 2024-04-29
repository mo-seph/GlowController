
#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
//#include "Arduino.h"
#include "ControlDefaults.h"
#include "GlowController.h"

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1


BaseController* glowControl;



static const char controlsJSON[] = ( R"(
{
  "controls":
  [
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
  ]
})"); 
static const char noControlsJSON[] = ( R"(
{"controls":[]})");

/*    {
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
    }*/


void setup() {
    while(!Serial) {delay(100);}
    Serial.println("Hello! Starting up... 2s delay");
    delay(2000);
    glowControl = new BaseController(strname(DEVICE_ID),strname(DEVICE_NAME));
    Serial.println("Initialising");
    glowControl->initialise();
    Serial.println("Initialised...");
    glowControl->setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    Serial.println("Setting up controls...");
    glowControl->setupControls(controlsJSON);
    Serial.println("Controls Setup");

}
int i = 0;
void loop() {
    glowControl->loop();
    //Serial.println(i++);
}
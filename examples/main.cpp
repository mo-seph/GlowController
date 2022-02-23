
//#include "GlowStrip.h"
#define FASTLED_ESP32_I2S true

#include "FastLEDGlowStrip.h"
//#include "GlowBehaviours.h"
#include "GlowController.h"
//#include "ArduinoJson.h"
#include "FastLED_RGBW.h"
#include "GlowMQTT.h"
#include "NTPFeature.h"
#include "WiFIFeature.h"
#include "SerialConnector.h"
#include "SPIFFS.h" 
#include "ControlManager.h"



DynamicJsonDocument doc(2000);
Controllers* controls;

static const char fullJson[] PROGMEM = ( R"(
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
    },
  ]
})");

void setup() {
    Serial.begin(115200);
    Serial.println("Hello!");
    deserializeJson(doc,fullJson);
    serializeJson(doc["controls"],Serial);
    controls = new Controllers(doc.as<JsonVariant>());
}

void loop() {
    controls->update();
    delay(200);
    Serial.println("Update...");
}
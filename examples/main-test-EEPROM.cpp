
#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
//#include "Arduino.h"
#include "ControlDefaults.h"
#include "GlowController.h"

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1

#include "strip/FastLEDGlowStrip.h"
#include "strip/FastLED_RGBW.h"
#include <ArduinoOTA.h>

FRGBW current[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 255;

FastLEDGlowStrip strip(NUM_LEDS,leds, current);
GlowController glowControl(&strip,strname(DEVICE_ID),strname(DEVICE_NAME));

static const char behaviourJSON[] PROGMEM = ( R"(
{
    "behaviours":
    [
      {
        "id":0,
        "type":"Fill",
        "name":"Baseline",
        "data":{"r":0.1,"b":0.0,"g":0.0,"w":0.3,"save":"base"}
      }
    ]
}
)");

static const char controlsJSON[] PROGMEM = ( R"( { "controls": [ ] })");

void setup() {
    FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));

    FastLED.setBrightness(brightness);
    strip.show();
    glowControl.initialise();
    glowControl.setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    //glowControl.setupControls();
    glowControl.setupBehaviours(behaviourJSON);

}

void loop() {
    glowControl.loop();
}
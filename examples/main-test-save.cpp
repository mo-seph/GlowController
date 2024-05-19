
#define FASTLED_ESP32_I2S true

#include "strip/FastLEDGlowStrip.h"
#include "ControlDefaults.h"
#include "GlowController.h"
#include "strip/FastLED_RGBW.h"

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1


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
      "id":6,
      "type":"Fire",
      "name":"Fire"
    }
  ]
}
)");


void setup() {
    //Serial.begin(115200);
    Serial.println("Starting");
      /* Serial version */
    FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
    /* Parallel version */
    //FastLED.addLeds<WS2812B, 12, RGB>(ledsRGB, getRGBWsize(LEDS_PER_STRIP));
    //FastLED.addLeds<WS2812B, 14, RGB>(ledsRGB + 4 * LEDS_PER_STRIP, getRGBWsize(LEDS_PER_STRIP));
    //FastLED.addLeds<WS2812B, 15, RGB>(ledsRGB + 8 * LEDS_PER_STRIP, getRGBWsize(LEDS_PER_STRIP));

    FastLED.setBrightness(brightness);
    strip.show();
    glowControl.initialise();
    glowControl.setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    //glowControl.setupControls();
    glowControl.setupBehaviours();
    //glowControl.setupBehaviours(behaviourJSON);

    //deserializeJson(doc,fullJson);
    //serializeJson(doc["controls"],Serial);
    //controls = new Controllers(doc.as<JsonVariant>());
}

void loop() {
    glowControl.loop();
    //delay(2000);
    //Serial.println("Update...");
}
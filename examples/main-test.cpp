
//#include "GlowStrip.h"
#define FASTLED_ESP32_I2S true

#include "strip/FastLEDGlowStrip.h"
//#include "GlowBehaviours.h"
#include "GlowController.h"
//#include "ArduinoJson.h"
#include "strip/FastLED_RGBW.h"
//#include "GlowMQTT.h"
//#include "NTPFeature.h"
//#include "WiFIFeature.h"
//#include "SerialConnector.h"
//#include "SPIFFS.h" 
//#include "ControlManager.h"


#ifndef NUM_LEDS
#define NUM_LEDS 100
#endif

#ifndef LED_DATA_PIN
#define LED_DATA_PIN 12
#endif

#define LEDS_PER_STRIP 400
#define NUM_STRIPS 3

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1

/* WIFI Defs */
const char* ssid = "Fishbowl2G";
const char* password = "29Goldfish";

/* MQTT Defs */
const char* mqtt_server = "192.168.178.108";
const int mqtt_port = 1883;

/* NTP Defs */
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset = 1;
const int   daylightOffset = 1;

//const char* id = "living";
//const char* name = "Living Room Lights";

#define str(x) #x
#define strname(name) str(name)

#ifndef DEVICE_ID
#define DEVICE_ID test
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME Test Device
#endif

//WiFiClient esp;
//PubSubClient client(esp);

FRGBW current[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 255;

FastLEDGlowStrip strip(NUM_LEDS,leds, current);
//GlowController glowControl(&strip,id,name);
GlowController glowControl(&strip,strname(DEVICE_ID),strname(DEVICE_NAME));

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
    },
  ]
})");

void setup() {
    Serial.begin(115200);
    Serial.println("Starting");
    glowControl.initialise();
    glowControl.setupBaseFeatures(ssid,password,mqtt_server,mqtt_port,gmtOffset,daylightOffset,ntpServer);

    //deserializeJson(doc,fullJson);
    //serializeJson(doc["controls"],Serial);
    //controls = new Controllers(doc.as<JsonVariant>());
}

void loop() {
    //controls->update();
    //delay(200);
    //Serial.println("Update...");
}
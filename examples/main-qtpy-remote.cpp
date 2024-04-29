#include <Arduino.h>

#define FASTLED_ESP32_I2S true

#include <ArduinoJson.h>
//#include "Arduino.h"
#include "ControlDefaults.h"
#include "GlowController.h"

BaseController glowControl(strname(DEVICE_ID),strname(DEVICE_NAME));


#include "Adafruit_seesaw.h"
#include <Adafruit_NeoPixel.h>
#define PIN        PIN_NEOPIXEL
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_seesaw ss(&Wire1);

#define BUTTON_X         6
#define BUTTON_Y         2
#define BUTTON_A         5
#define BUTTON_B         1
#define BUTTON_SELECT    0
#define BUTTON_START    16
//uint32_t button_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) |
 //                      (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_SELECT);

//#define IRQ_PIN   5

static const char controlsJSON[] PROGMEM = ( R"(
{
  "controls":
  [
  
  ]
})");


void setup() {
    glowControl.initialise();
    glowControl.setupBaseFeatures(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    glowControl.setupControls(controlsJSON);
  //Serial.begin(115200);

  while(!Serial) {
    delay(10);
  }

  Serial.println("Gamepad QT example!");
  
  if(!ss.begin(0x50)){
    Serial.println("ERROR! seesaw not found on 0x50");
    while(1) delay(1);
  }
  Serial.println("seesaw started");
  uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
  if (version != 5743) {
    Serial.print("Wrong firmware loaded? ");
    Serial.println(version);
    while(1) delay(10);
  }
  Serial.println("Found Product 5743");
  
  ss.pinModeBulk(GAMEPAD_BUTTON_MASK, INPUT_PULLUP);
  ss.setGPIOInterrupts(GAMEPAD_BUTTON_MASK, 1);

#if defined(IRQ_PIN)
  pinMode(IRQ_PIN, INPUT);
#endif
  pixels.begin();
}


int last_x = 0, last_y = 0;

void loop() {
  //delay(10); // delay in loop to slow serial output
    glowControl.loop();
  
  // Reverse x/y values to match joystick orientation
  int x = 1023 - ss.analogRead(14);
  int y = 1023 - ss.analogRead(15);
  
  if ( (abs(x - last_x) > 3)  ||  (abs(y - last_y) > 3)) {
    Serial.print("x: "); Serial.print(x); Serial.print(", "); Serial.print("y: "); Serial.println(y);
    last_x = x;
    last_y = y;
  }
  pixels.setPixelColor(0, pixels.Color(x /4, y/4, 0));
  pixels.show();

#if defined(IRQ_PIN)
  if(!digitalRead(IRQ_PIN)) {
    return;
  }
#endif

    uint32_t buttons = ss.digitalReadBulk(GAMEPAD_BUTTON_MASK);

    //Serial.println(buttons, BIN);

    if (! (buttons & (1UL << BUTTON_A))) {
      Serial.println("Button A pressed");
    }
    if (! (buttons & (1UL << BUTTON_B))) {
      Serial.println("Button B pressed");
    }
    if (! (buttons & (1UL << BUTTON_Y))) {
      Serial.println("Button Y pressed");
    }
    if (! (buttons & (1UL << BUTTON_X))) {
      Serial.println("Button X pressed");
    }
    if (! (buttons & (1UL << BUTTON_SELECT))) {
      Serial.println("Button SELECT pressed");
    }
    if (! (buttons & (1UL << BUTTON_START))) {
      Serial.println("Button START pressed");
    }
}
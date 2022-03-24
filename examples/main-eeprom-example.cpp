#include "Preferences.h"
#include "ColorStructures.h"

Preferences prefs;

void setup() {
    Serial.begin(115200);
    delay(1400);
    prefs.begin("colortest");
    FRGBW initial = FRGBW(0.0,0.25,0.5,0.75);
    FRGBW toStore = FRGBW(1.0,0.75,0.5,0.25);

    initial.toSerial();
    prefs.getBytes("incol",&initial,sizeof(FRGBW));
    initial.toSerial();
    prefs.putBytes("incol",&toStore,sizeof(FRGBW));
    initial.toSerial();
    prefs.getBytes("incol",&initial,sizeof(FRGBW));
    initial.toSerial();
}

void loop() {
  /* Empty loop */
}
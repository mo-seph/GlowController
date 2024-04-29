#include "Arduino.h"

// Pins to avoid 22,21 (SCL,SDA); 1,3 (Rx/Tx)
// 0 seem strange on ADC
// 2,4,25,27,32 all OK
int jx1 = 2; //25
int jy1 = 4; //2
int jx2 = 27;
int jy2 = 32;
int b1 = 0;
int b2 = 25;

void setup() {
    Serial.begin(115200);
    pinMode(b1,INPUT_PULLUP);
    pinMode(b2,INPUT_PULLUP);
}

void loop() {
    Serial.print("> ");
    Serial.print("\tX1: "); Serial.print(analogRead(jx1)/4096.0);
    Serial.print("\tY1: "); Serial.print(analogRead(jy1)/4096.0);
    Serial.print("\tX2: "); Serial.print(analogRead(jx2)/4096.0);
    Serial.print("\tY2: "); Serial.print(analogRead(jy2)/4096.0);
    Serial.print("\tB1: "); Serial.print(!digitalRead(b1));
    Serial.print("\tB2: "); Serial.print(!digitalRead(b2));
    Serial.println();
    delay(100);

}
#include "Arduino.h"
#include "ControlDefaults.h"


void commsSetup(const char* ssid, const char* password, 
    const char* mqtt_server, int mqtt_port,
    int gmtOffset, int daylightOffset,const char* ntp_server) {
    Serial.println("+++ Setting up WiFi: ");
    Serial.print("SSID: "); Serial.println(ssid);
    Serial.print("Password: "); Serial.println(password);
    Serial.print("MQTT Server: "); Serial.println(mqtt_server);
    Serial.print("MQTT Port: "); Serial.println(mqtt_port);
    Serial.print("GMT Offset: "); Serial.println(gmtOffset);
    Serial.print("Daylight Offset: "); Serial.println(daylightOffset);
    Serial.print("NTP Server: "); Serial.println(ntp_server);
}
void deviceSetup(const char* id, const char* name) {
    Serial.println("+++ Setting up Device: ");
    Serial.print("Device Name: "); Serial.println(id);
    Serial.print("Device ID: "); Serial.println(name);
}
void ledSetup(int numLEDs) {
    Serial.println("+++ Setting up LEDS: ");
    Serial.print("Num LEDs: "); Serial.println(numLEDs);
}

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 4000 ) {}
    deviceSetup(strname(DEVICE_NAME),strname(DEVICE_ID));
    commsSetup(strname(WIFI_SSID), strname(WIFI_PASSWORD), strname(MQTT_SERVER), MQTT_PORT, GMT_OFFSET, DAYLIGHT_OFFSET, strname(NTP_SERVER));
    ledSetup(NUM_LEDS);
}

void loop() {

}
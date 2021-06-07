#ifndef GLOW_MQTT_H
#define GLOW_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "GlowController.h"
#include "ArduinoJson.h"

void setupWiFi();
void sendJsonMQTT(DynamicJsonDocument doc );
void callback(char* topic, byte* payload, unsigned int length);
void setupMQTT(GlowController* c);
void mqttConnect();
void setupGlowMQTT(GlowController* gc, const char* ssid, const char* pw, const char* server, int port);




#endif

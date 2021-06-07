#ifndef GLOW_MQTT_H
#define GLOW_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "GlowController.h"
#include "ArduinoJson.h"

//GlowController glowControl;

/* Set up WiFi */

/* Set up MQTT */
WiFiClient espClient;
PubSubClient client(espClient);
#define MSG_BUFFER_SIZE	(1000)
char msg[MSG_BUFFER_SIZE];

//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* command_channel = "/leds/1/commands";
const char* state_channel = "/leds/1/state";

long retry_time = 0;
long connection_interval = 2000;

GlowController* MQTT_GLOW_CONTROL = NULL;



void setupWiFi(const char* ssid, const char* password) {
  Serial.println("Setting up WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");
  Serial.println(WiFi.localIP());
}

void sendJsonMQTT(DynamicJsonDocument doc ) {
  if( client.connected() ) {
    Serial.print("Sending state on ["); Serial.print(state_channel); Serial.println("]");
    serializeJson(doc, msg);
    client.publish(state_channel, msg);
  } else {
    Serial.println("Skipping MQTT output - not connected");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  MQTT_GLOW_CONTROL->update(payload,length);
}


void setupMQTT(GlowController* c, const char * mqtt_server, int mqtt_port) {
  client.setBufferSize(1000);
  MQTT_GLOW_CONTROL = c;
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


void mqttConnect() {
  /*
  Try making a connection if we don't have one, but don't wait, get ready
  to try again later
  */
  if(!client.connected() ) {
    if( retry_time < millis() ) {
      Serial.print("Attempting MQTT connection...");
      // Create a random client ID
      String clientId = "GlowLights-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str())) {
        Serial.println("MQTT connected");
        // Once connected, publish an announcement...
        client.publish(state_channel, "{\"connected\":1}");
        // ... and resubscribe
        client.subscribe(command_channel);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 2 seconds");
      }
    } else {
      retry_time = millis() + connection_interval;
    }
  }

  /* IFF we have a connection, then run the MQTT loop */
  if( client.connected() )   client.loop();
}

void setupGlowMQTT(GlowController* gc,
    const char* ssid, const char* pw, const char* server, int port = 1883)
  {
    setupWiFi(ssid, pw);
    setupMQTT(gc,server,port);
    mqttConnect();
  }

#endif

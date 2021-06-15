#include <WiFi.h>
#include <PubSubClient.h>
#include "GlowController.h"
#include "ArduinoJson.h"

#ifndef GLOW_MQTT_H
#include "GlowMQTT.h"
#endif

//GlowController glowControl;

//GlowController* MQTT_GLOW_CONTROL = NULL;


/* Set up WiFi */

/* Set up MQTT */
WiFiClient espClient;
static PubSubClient client(espClient);
#define MSG_BUFFER_SIZE	(1400)
char msg[MSG_BUFFER_SIZE];



//const char* mqtt_server = "broker.mqtt-dashboard.com";
//const char* command_channel = "leds/1/commands";
//const char* state_channel = "leds/1/state";

long retry_time = 0;
long connection_interval = 500;

//GlowController* MQTT_GLOW_CONTROL = NULL;




/*
void sendJsonMQTT(DynamicJsonDocument doc ) {
  if( client.connected() ) {
    Serial.print("Sending state on ["); Serial.print(state_channel); Serial.println("]");
    serializeJson(doc, msg);
    client.publish(state_channel, msg);
  } else {
    Serial.println("Skipping MQTT output - not connected");
  }
}
*/

void callback(char* topic, byte* payload, unsigned int length) {
  if( strcmp(topic,PING_REQ_CHANNEL) == 0 ) {
    MQTTConnector::gc_s->ping();
  }
  else {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    MQTTConnector::gc_s->update(payload,length);
  }
}

GlowController* MQTTConnector::gc_s;
MQTTConnector* MQTTConnector::mt_s;

/*
void setupMQTT(GlowController* c, const char * mqtt_server, int mqtt_port) {
  client.setBufferSize(1000);
  MQTT_GLOW_CONTROL = c;
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
*/


  /*
  Try making a connection if we don't have one, but don't wait, get ready
  to try again later
  */
  /*
void mqttConnect() {
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

  // IFF we have a connection, then run the MQTT loop
  if( client.connected() )   client.loop();
}
*/

/*
void setupGlowMQTT(GlowController* gc,
    const char* ssid, const char* pw, const char* server, int port = 1883)
  {
    setupWiFi(ssid, pw);
    setupMQTT(gc,server,port);
    mqttConnect();
  }
  */

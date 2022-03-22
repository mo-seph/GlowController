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
//WiFiClient espClient;
//static PubSubClient client(espClient);
#define MSG_BUFFER_SIZE	(3400)
char msg[MSG_BUFFER_SIZE];

long retry_time = 0;
long connection_interval = 500;


/*
 * We need a callback that is a plain function; hence, we store a static
 * instance of the connector (should only ever be one, really!) and the
 * callback references that
 */
void callback(char* topic, byte* payload, unsigned int length) {
  MQTTConnector::static_mqtt->gotInput(topic, payload,length);

}

MQTTConnector* MQTTConnector::static_mqtt;

  void MQTTConnector::setController(BaseController *c) {
    Serial.println("Setting controller for MQTT Connector");
    controller = c;
    //MQTTConnector::static_controller = c;
    MQTTConnector::static_mqtt = this;
    client_id = String("GlowLight-") + c->getID();
    command_channel = String("leds/") + c->getID() + "/commands";
    Serial.print("Command channel: "); Serial.println(command_channel);
    state_channel = String("leds/") + c->getID() + "/state";
    Serial.print("State channel: "); Serial.println(state_channel);


    client->setCallback(callback);
    client->subscribe(command_channel.c_str());
    Serial.println("Set callback function on client");
  };

  void MQTTConnector::updateConnection() {
    /*
    Try making a connection if we don't have one, but don't wait, get ready
    to try again later
    */
    if(!client->connected() ) {
      if( millis() > last_update + connection_interval ) {
        last_update = millis();
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client->connect(client_id.c_str())) {
          Serial.print("MQTT connected publishing on: ");
          Serial.print(state_channel);
          Serial.print(", listening on: ");
          Serial.println(command_channel);
          controller->pingDoc()["mqtt_commands"] = command_channel;
          controller->pingDoc()["mqtt_state"] = state_channel;
          controller->sendState();
          // ... and resubscribe
          client->subscribe(command_channel.c_str());
          client->subscribe(PING_REQ_CHANNEL);
          // Once connected, publish an announcement...
          controller->ping();
        } else {
          Serial.print("failed, rc=");
          Serial.print(client->state());
          Serial.println(" try again in 2 seconds");
        }
      } else {
        //retry_time = millis() + connection_interval;
      }
    }
  }
void MQTTConnector::gotInput(char* topic, byte* payload, unsigned int length) {
    if( strcmp(topic,PING_REQ_CHANNEL) == 0 ) {
      controller->ping();
    }
    else {
      /*
      Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
      for (int i = 0; i < length; i++) { Serial.print((char)payload[i]); }
      Serial.println();
      */
      DynamicJsonDocument input(3000);
      if( checkDeserialisation( deserializeJson(input, payload, length) ) )
        controller->processInput(input.as<JsonVariant>());
    }
  }
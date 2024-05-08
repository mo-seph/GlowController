#include <WiFi.h>
#include <PubSubClient.h>
#include "GlowController.h"
#include "ArduinoJson.h"

#ifndef GLOW_MQTT_H
#include "GlowMQTT.h"
#endif

//GlowController glowControl;

//GlowController* MQTT_GLOW_CONTROL = NULL;
MQTTConnector* GLOBAL_MQTT_CONNECTOR = NULL;


/* Set up WiFi */

/* Set up MQTT */
//WiFiClient espClient;
//static PubSubClient client(espClient);
#define MSG_BUFFER_SIZE	(3400)
char msg[MSG_BUFFER_SIZE];

long retry_time = 0;
long connection_interval = 500;
const int MAX_CONNECTION_INTERVAL = 10 * 60 * 1000;


/*
 * We need a callback that is a plain function; hence, we store a static
 * instance of the connector (should only ever be one, really!) and the
 * callback references that
 */
void callback(char* topic, byte* payload, unsigned int length) {
  GLOBAL_MQTT_CONNECTOR->gotInput(topic, payload,length);

}

//MQTTConnector* MQTTConnector::static_mqtt;

void MQTTConnector::setController(BaseController *c) {
    Connector::setController(c);
    controller = c;
    Serial.println("Setting controller for MQTT Connector");
    Serial.print("Setting server ["); Serial.print(mqtt_server); Serial.print("] and port ["); Serial.print(mqtt_port); Serial.println("]");

    client_id = String("GlowLight-") + c->getID();
    Serial.print("Got ID: ");
    Serial.println(client_id);
    
    client->setServer(mqtt_server, mqtt_port);
    updateConnection();

    //MQTTConnector::static_controller = c;
    //MQTTConnector::static_mqtt = this;
    GLOBAL_MQTT_CONNECTOR = this;
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
      if( (millis() > last_update + connection_interval ) && WiFi.isConnected() ) {
        last_update = millis();
        connection_interval *= 2;
        if( connection_interval > MAX_CONNECTION_INTERVAL ) connection_interval = MAX_CONNECTION_INTERVAL;
        Serial.print("Attempting MQTT connection as");
        Serial.println(client_id.c_str());
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
          Serial.print(" try again in ");
          Serial.print(connection_interval / 1000);
          Serial.println(" seconds");
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
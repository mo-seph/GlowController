#ifndef GLOW_MQTT_H
#define GLOW_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "GlowController.h"
#include "ArduinoJson.h"
#include "GlowInterfaces.h"

#define MSG_BUFFER_SIZE	(1000)

#define PING_CHANNEL "leds/ping"


//GlowController* MQTT_GLOW_CONTROL;

//void setupWiFi(const char* ssid, const char* password);
//void sendJsonMQTT(DynamicJsonDocument doc );
//void setupMQTT(GlowController* c);
//void mqttConnect();
//void setupGlowMQTT(GlowController* gc, const char* ssid, const char* pw, const char* server, int port);

void callback(char* topic, byte* payload, unsigned int length);




class MQTTConnector : public Connector {
public:
  MQTTConnector(PubSubClient *client, const char* mqtt_server, int mqtt_port) :
    Connector(), client(client), mqtt_server(mqtt_server), mqtt_port(mqtt_port), ping_response(300)
      //command_channel("leds/1/commands"), state_channel("leds/1/state")
      {
      Serial.println("Creating MQTT Connector");
      client->setBufferSize(1000);
      Serial.print("Setting server ["); Serial.print(mqtt_server); Serial.print("] and port ["); Serial.print(mqtt_port); Serial.println("]");
      client->setServer(mqtt_server, mqtt_port);
      updateConnection();

  }
  static GlowController* gc_s;
  static MQTTConnector* mt_s;

  /* Should call super; lazy... */
  virtual void setController(GlowController *c) {
    Serial.println("Setting controller for MQTT Connector");
    controller = c;
    MQTTConnector::gc_s = c;
    MQTTConnector::mt_s = this;
    client_id = String("GlowLight-") + c->getID();
    command_channel = String("leds/") + c->getID() + "/commands";
    state_channel = String("leds/") + c->getID() + "/state";
    ping_response["id"] = controller->getID();
    ping_response["name"] = controller->getName();


    client->setCallback(callback);
    client->subscribe(command_channel.c_str());
    Serial.println("Set callback function on client");
  };

  void updateConnection() {
    /*
    Try making a connection if we don't have one, but don't wait, get ready
    to try again later
    */
    if(!client->connected() ) {
      if( retry_time < millis() ) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client->connect(client_id.c_str())) {
          Serial.print("MQTT connected publishing on ");
          Serial.println(state_channel);
          // Once connected, publish an announcement...
          ping();
          controller->sendState();
          // ... and resubscribe
          client->subscribe(command_channel.c_str());
          client->subscribe(PING_CHANNEL);
        } else {
          Serial.print("failed, rc=");
          Serial.print(client->state());
          Serial.println(" try again in 2 seconds");
        }
      } else {
        retry_time = millis() + connection_interval;
      }
    }

  }

  void ping() {
    if( client->connected() ) {
      serializeJson(ping_response, msg);
      client->publish(state_channel.c_str(), msg);
    }
  }


  virtual void update(DynamicJsonDocument doc) {
    updateConnection();
    /* IFF we have a connection, then run the MQTT loop */
    if( client->connected() )   client->loop();
  }

  virtual void outputState(JsonVariant v) {
    if( client->connected() ) {
      Serial.print("Sending state on ["); Serial.print(state_channel); Serial.println("]");
      serializeJson(v, msg);
      client->publish(state_channel.c_str(), msg);
    } else {
      Serial.println("Skipping MQTT output - not connected");
    }
  }
protected:
  //WiFiClient wifi_client;
  PubSubClient *client;

  String client_id = "GlowLight-";
  String command_channel = "leds/";
  String state_channel = "leds/";
  String ping_msg = String("{\"connected\":0}");


  char msg[MSG_BUFFER_SIZE];
  //const char* mqtt_server = "broker.mqtt-dashboard.com";
  const char* mqtt_server;
  int mqtt_port;
  //const char* command_channel;
  //const char* state_channel;

  long retry_time = 0;
  long connection_interval = 2000;
  DynamicJsonDocument ping_response;

};


#endif

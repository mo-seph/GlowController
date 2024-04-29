#ifndef GLOW_MQTT_H
#define GLOW_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "BaseController.h"
#include "ArduinoJson.h"
#include "connectors/GlowConnectors.h"

#define MSG_BUFFER_SIZE	(3400)

#define PING_REQ_CHANNEL "leds/ping_request"
#define PING_RESP_CHANNEL "leds/ping_response"


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
      client->setBufferSize(1400);

  }
  //static GlowController* static_controller;
  static MQTTConnector* static_mqtt;

  /* Should call super; lazy... */
  virtual void setController(BaseController *c); 
  void updateConnection(); 

  virtual void ping(JsonVariant doc) {
    if( client->connected() ) {
      Serial.println("MQTT Pinging");
      serializeJson(doc, msg);
      client->publish(PING_RESP_CHANNEL, msg);
    }
  }


  virtual void update() {
    updateConnection();
    /* IFF we have a connection, then run the MQTT loop */
    if( client->connected() )   client->loop();
  }

  void gotInput(char* topic, byte* payload, unsigned int length); 

  virtual void outputState(JsonVariant v) {
    sendJSON(state_channel.c_str(),v);
  }

  void sendJSON(const char* channel, JsonVariant v) {
    if( client->connected() ) {
      Serial.print("Sending doc on ["); Serial.print(channel); Serial.println("]");
      serializeJson(v, msg);
      client->publish(channel, msg);
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

  long last_update = 0;
  long connection_interval = 2000;
  DynamicJsonDocument ping_response;
};


#endif

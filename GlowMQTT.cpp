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

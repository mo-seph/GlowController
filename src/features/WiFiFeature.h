#ifndef WIFI_FEATURE_H
#define WIFI_FEATURE_H

#include "features/GlowFeature.h"
#include "time.h"

//Note - this needs to be incuded in the main file too for some reason?
#include <ArduinoOTA.h>



class WiFiFeature : public Feature  {
public:
  WiFiFeature(const char* ssid, const char* password ) :
    Feature(), ssid(ssid), password(password), connected(false), trying(false) {
  };

  virtual void update() {
    if(!connected && !trying ) {
      trying = true;
      Serial.println("Setting up WiFi");
      WiFi.begin(ssid, password);
    }
    if( trying && WiFi.status() == WL_CONNECTED ) {
      trying = false;
      connected = true;
      Serial.println("Connected to network");
      Serial.println(WiFi.localIP());
      controller->pingDoc()["ip"] = WiFi.localIP();
      controller->ping();
      ArduinoOTA.setHostname(controller->getID());
      ArduinoOTA.begin();
    }
    if( connected ) {
      ArduinoOTA.handle();
    }
  };

protected:
  const char* ssid;
  const char* password;
  bool connected;
  bool trying;
};

#endif

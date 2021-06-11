#ifndef NTP_FEATURE_H
#define NTP_FEATURE_H

#include "GlowInterfaces.h"
#include "time.h"

void ntpCallback(timeval*) {
  Serial.println("NTP UPDATED");
}

class NTPFeature : public Feature  {
public:
  NTPFeature(const char* ntpServer, const long gmtOffset, const int daylightOffset ) :
    Feature(), ntpServer(ntpServer), gmtOffset(gmtOffset), daylightOffset(daylightOffset) {
      configTime(gmtOffset, daylightOffset, ntpServer);
      sntp_set_time_sync_notification_cb(ntpCallback);
  };

  virtual void update() {
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo,50)){
        //Serial.println("Failed to obtain time");
        return;
      } else {
        controller->setTime(timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
        controller->setDate(timeinfo.tm_mday,timeinfo.tm_mon,timeinfo.tm_year);
      }

    }

protected:
  const char* ntpServer;
  const long  gmtOffset;
  const int   daylightOffset;
};

#endif

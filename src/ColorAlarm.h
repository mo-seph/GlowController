#ifndef ColorAlarm_h
#define ColorAlarm_h
#include "Arduino.h"
#include "GlowStrip.h"
#include <ArduinoJson.h>
#include <GlowController.h>

#include "TimeStruct.h"

class ColorAlarm : public GlowBehaviour {
public:
  ColorAlarm(GlowController* s) :
    GlowBehaviour(s,"ColorAlarm"),
    startColor(0,1,0,0), endColor(1,0,0,0),
    startPos(0.03), endPos(0.8),
    timeBefore(15), timeAfter(10),
    breath(s->getStrip(), endColor, 3.5) {
    breath.activate();
  }

  void doUpdate(long updateTime) {
    int startPixel = strip->positionToPixels(startPos);
    int endPixel = strip->positionToPixels(endPos);
    breath.setRange(startPixel, endPixel);

    //float minutesLeft = 0.5;
    //Time in minutes relative to the planned alarm. Positive means it is before the alarm
    //float relativeTime = 20.0 - ((float)millis() / 1000.0);
    float relativeTime = getRelativeTime();

    if( relativeTime > timeBefore ) {
      // Then it is too early to start
      return;
    } else if (relativeTime < -timeAfter ) {
      //Now it's too late to do anything
      return;
    } else if( relativeTime >= 0 ) {
      //This is the main countdown, fade from red to green...
      FRGBW color = strip->interpolateRGBW(endColor, startColor, relativeTime / timeBefore);
      strip->fillRGBW(color, startPixel, endPixel);
    } else {
      //The time is passed! Flash!
      breath.doUpdate(updateTime);
    }
  }

  //Time in minutes relative to the planned alarm. Positive means it is before the alarm
  //This is using hacky version of local time as the time libraries are confusing!
  float getRelativeTime() {
    if( ! controller->timeSet() ) { return 60000; }
    CurrentTime* cur = controller->getTime();
    float currentMinutes = cur->hour * 60.0 + (float)cur->minute + cur->second / 60.0;
    float targetMinutes = hour * 60.0 + (float)minute;
    float timeLeft = targetMinutes - currentMinutes;
    // Deal with wraparound at midnight
    if( timeLeft < -timeAfter ) timeLeft += (24*60.0);

    /*
    if( timedOutput(1000) ) {
      Serial.print("Current time: ");Serial.print(cur->hour);Serial.print(":");Serial.print(cur->minute);Serial.print(":");Serial.print(cur->second);
      Serial.print(" = "); Serial.print(currentMinutes);
      Serial.print(", Target time: ");Serial.print(hour);Serial.print(":");Serial.print(minute);
      Serial.print(" = "); Serial.print(targetMinutes);
      Serial.print(", Time left: "); Serial.println(timeLeft);
    }
    */
    return timeLeft;
  }


  void stateFromJson(JsonVariant d) {
    if( d.containsKey("hour")) hour = d["hour"];
    if( d.containsKey("minute")) minute = d["minute"];
    if( d.containsKey("start")) startPos = d["start"];
    if( d.containsKey("end")) endPos = d["end"];
    if( d.containsKey("timeBefore")) timeBefore = d["timeBefore"];
    if( d.containsKey("timeAfter")) timeAfter = d["timeAfter"];
  }


  void stateToJson(JsonVariant d) {
    //targetColor.toJson(d);
    d["start"] = startPos;
    d["end"] = endPos;
    d["hour"] = hour;
    d["minute"] = minute;
    d["timeBefore"] = timeBefore;
    d["timeAfter"] = timeAfter;
  }


protected:
  FRGBW startColor;
  FRGBW endColor;
  float startPos;
  float endPos;
  float timeBefore;
  float timeAfter;
  Breath breath;
  int hour;
  int minute;
};

#endif

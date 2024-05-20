#ifndef GlowAlarms_h
#define GlowAlarms_h
#include "Arduino.h"
#include "strip/GlowStrip.h"
#include <ArduinoJson.h>
#include <GlowController.h>
#include "behaviours/GlowBehaviours.h"

#include "TimeStruct.h"


struct Alarm {
  long start_time;
  long end_time;
  float start_point;
  float length;
  FRGBW count_color;
  FRGBW frame_color;
  bool running;
  Alarm(float start,float length,FRGBW col,FRGBW frame) {
    start_time = -1;
    end_time = -1;
    start_point = start;
    length = length;
    count_color = col;
    frame_color = frame;
    running = false;
  };
};

class PixelCountdown : public GlowBehaviour {
public:
  PixelCountdown(GlowController* s) :
    GlowBehaviour(s,"PixelCountdown"),
    alarm(0.05,0.5,FRGBW(1,0,0,0),FRGBW(0,1,1,0)),
    breath(s->getStrip(), FRGBW(0,1,1,0), 3.5) {
    setStart(0.03);
    setLength(0.5);
    alarm.running = false;
  }
  /*
  Fill(GlowStrip* s, FRGBW col) :
    Fill(s), currentColor(col), targetColor(col) { }
    */

  void doUpdate(long updateTime) {
    drawAlarm(&alarm);
    breath.doUpdate(updateTime);
  }

  void drawAlarm(Alarm *a) {
    if(! a->running ) { return; }
    long current_time = millis();
    if( current_time > a->end_time ) {
      a->running = false;
      Serial.println("Alarm finished!");
      controller->sendState();
      breath.setCycles(5);
    }
    int border = 1;
    long total_time = a->end_time - a->start_time;
    long elapsed_time = current_time - a->start_time;
    float time_proportion = 1 - (float)(elapsed_time)/(float)total_time;

    strip->drawBar(
      strip->positionToPixels(a->start_point),
      strip->positionToPixels(a->start_point+a->length), 
      a->count_color,
      time_proportion,
      border,
      FRGBW(0,0,0.4,0.8),
      FRGBW(0.5,0.1,0.1,0.2));

  }

/*
{"update":6, "data":{"time":23}}
{"update":6, "data":{"running":true}}
*/

  void stateFromJson(JsonVariant d) {
    if( d.containsKey("time")) setCountdownTime((int)d["time"].as<float>());
    if( d.containsKey("position")) setStart(d["position"]);
    if( d.containsKey("length")) setLength(d["length"]);
    if( d.containsKey("running")) {
      Serial.print("Got key for running: "); serializeJson(d["running"],Serial); Serial.println();
      if( d["running"]) startCountdown();
      else stopCountdown();
    }
  }

  void setCountdownTime(int seconds) {
    countdownTime = seconds;
  }

  void startCountdown() {
    active = true;
    alarm.start_time = millis();
    alarm.end_time = millis() + countdownTime * 1000;
    Serial.print("Starting countdown. Setting end time as "); Serial.println(alarm.end_time);
    alarm.running = true;
  }

  void stopCountdown() {
      alarm.running = false;
  }

  void setStart(float start) {
    alarm.start_point = start;
    int startPixel = strip->positionToPixels(alarm.start_point);
    int endPixel = strip->positionToPixels(alarm.length) + startPixel;
    breath.setRange(startPixel, endPixel);
  }

  void setLength(float length) {
    alarm.length = length;
    int startPixel = strip->positionToPixels(alarm.start_point);
    int endPixel = strip->positionToPixels(length) + startPixel;
    breath.setRange(startPixel, endPixel);
  }

  void stateToJson(JsonVariant d) {
    //targetColor.toJson(d);
    d["running"] = alarm.running;
    d["position"] = alarm.start_point;
    d["length"] = alarm.length;
    d["time"] = countdownTime;
    //d["time"] = (nextInterpTime > 0) ? nextInterpTime : interpTime;
  }

protected:
  Alarm alarm;
  Breath breath;
  int countdownTime; // Time to count down in seconds
};

#endif

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

    int countdown_length_pixels = strip->positionToPixels(a->length) - 2*border;
    float size = time_proportion * countdown_length_pixels;
    int num_pixels = floor(size);
    float frac = size - (int)num_pixels;
    /*
    Serial.print("Start at: "); Serial.print(a->start_time);
    Serial.print(", Finish at: "); Serial.print(a->end_time);
    Serial.print(", current: "); Serial.print(current_time);
    Serial.print(", length: "); Serial.print(a->length);
    Serial.print(", countdown_pixels: "); Serial.print(countdown_length_pixels);
    Serial.print(", proportion: "); Serial.print(time_proportion);
    Serial.print(", num pixels: "); Serial.print(num_pixels);
    Serial.println();
    */
    int start_pixel = strip->positionToPixels(a->start_point);
    strip->startPixels(start_pixel,1);
    for(int i = 0; i < border; i++ ) { strip->addPixel(a->frame_color);}
    for(int i = 0; i < countdown_length_pixels; i++ ) {
      if( i < num_pixels ) strip->addPixel(a->count_color);
      else if( i == num_pixels ) strip->addPixel(interpolateRGBW(FRGBW(0,0,0,0),a->count_color,frac));
      else strip->addPixel(FRGBW(0,0,0,0));
    }
    for(int i = 0; i < border; i++ ) { strip->addPixel(a->frame_color);}
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
      alarm.running = true;
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

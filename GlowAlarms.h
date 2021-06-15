#ifndef GlowAlarms_h
#define GlowAlarms_h
#include "Arduino.h"
#include "GlowStrip.h"
#include <ArduinoJson.h>
#include <GlowController.h>

#include "TimeStruct.h"


struct Alarm {
  long start_time;
  long end_time;
  float start_point;
  int length;
  FRGBW count_color;
  FRGBW frame_color;
  bool running;
  Alarm(float start,int length,FRGBW col,FRGBW frame) {
    start_time = -1;
    end_time = -1;
    start_point = start;
    length = length;
    count_color = col;
    frame_color = frame;
    running = false;
  };
};

class Alarms : public GlowBehaviour {
public:
  Alarms(GlowController* s) : GlowBehaviour(s,"Alarms"),  alarm(0.03,30,FRGBW(1,0,0,0),FRGBW(0,1,1,0)) {}
  /*
  Fill(GlowStrip* s, FRGBW col) :
    Fill(s), currentColor(col), targetColor(col) { }
    */

  void doUpdate(long updateTime) {
    drawAlarm(&alarm);
  }

  void drawAlarm(Alarm *a) {
    if(! a->running ) { return; }
    long current_time = millis();
    if( current_time > a->end_time ) {
      a->running = false;
      Serial.println("Alarm finished!");
      controller->sendState();
    }
    int border = 1;
    int strip_length = a->length - 2*border;
    long total_time = a->end_time - a->start_time;
    long elapsed_time = current_time - a->start_time;
    float proportion = 1 - (float)(elapsed_time)/(float)total_time;
    float size = proportion * strip_length;
    int num_pixels = floor(size);
    float frac = size - (int)num_pixels;
    /*
    Serial.print("Start at: "); Serial.print(a->start_time);
    Serial.print(", Finish at: "); Serial.print(a->end_time);
    Serial.print(", current: "); Serial.print(current_time);
    Serial.print(", proportion: "); Serial.print(proportion);
    Serial.print(", num pixels: "); Serial.print(num_pixels);
    Serial.println();
    */
    int start_pixel = strip->positionToPixels(a->start_point);
    strip->startPixels(start_pixel,1);
    for(int i = 0; i < border; i++ ) { strip->addPixel(a->frame_color);}
    for(int i = 0; i < strip_length; i++ ) {
      if( i < num_pixels ) strip->addPixel(a->count_color);
      else if( i == num_pixels ) strip->addPixel(strip->interpolateRGBW(FRGBW(0,0,0,0),a->count_color,frac));
      else strip->addPixel(FRGBW(0,0,0,0));
    }
    for(int i = 0; i < border; i++ ) { strip->addPixel(a->frame_color);}
  }


  void stateFromJson(JsonVariant d) {
    if( d.containsKey("time")) {
      alarm.start_time = millis();
      alarm.end_time = millis() + (int)(d["time"].as<float>() * 1000 );
      alarm.running = true;
      Serial.print("Setting end time as "); Serial.println(alarm.end_time);
    }
    if( d.containsKey("start")) alarm.start_point = d["start"];
    if( d.containsKey("length")) alarm.length = d["length"];
    if( d.containsKey("running")) alarm.running = d["running"];
  }

  void stateToJson(JsonVariant d) {
    //targetColor.toJson(d);
    d["running"] = alarm.running;
    d["start"] = alarm.start_point;
    d["length"] = alarm.length;
    //d["time"] = (nextInterpTime > 0) ? nextInterpTime : interpTime;
  }

protected:
  Alarm alarm;

};

#endif

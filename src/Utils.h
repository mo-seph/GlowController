#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"
#include <ArduinoJson.h>

/*

*/

static float clamp(float v, float min=0.0, float max=1.0 ) {
    if( v > max ) return max;
    if( v < min ) return min;
    return v;
}

static float wrap(float v, float max ) {
    if( v > max ) return fmod(v, max);
    return v;
}

static float mapF(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif
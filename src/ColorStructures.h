#ifndef COLOR_STRUCTURES_H
#define COLOR_STRUCTURES_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Utils.h"
//#define DEG_TO_RAD(X) (M_PI*(X)/180)

/**
* This is a float based data container for LED strips
* Absolute waste of memory, but makes some things easier
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


struct FRGBW  {
  union {
    struct {
      union {
        float g;
        float green;
      };
      union {
        float r;
        float red;
      };
      union {
        float b;
        float blue;
      };
      union {
        float w;
        float white;
      };
    };
    float raw[4];
  };

  FRGBW(){}

  FRGBW(float rd, float grn, float blu, float wht){
    r = rd;
    g = grn;
    b = blu;
    w = wht;
  }

  void fromJson(JsonVariant d) {
    if( d.containsKey("r")) r = clamp(d["r"]);
    if( d.containsKey("g")) g = clamp(d["g"]);
    if( d.containsKey("b")) b = clamp(d["b"]);
    if( d.containsKey("w")) w = clamp(d["w"]);
  };

  void toJson(JsonVariant d) {
    d["r"] = r;
    d["g"] = g;
    d["b"] = b;
    d["w"] = w;
  }
  void toSerial() {
    Serial.print("RGBW :: r: "); Serial.print(r); Serial.print(", g: "); Serial.print(g); Serial.print(", b: "); Serial.print(b); Serial.print(", w: "); Serial.print(w);
    Serial.println();
  }
};

/**
* This is a float based data container for LED strips
* Absolute waste of memory, but makes some things easier
*/

struct FHSV  {
  union {
    struct {
      union {
        float h;
        float hue;
      };
      union {
        float s;
        float saturation;
      };
      union {
        float v;
        float value;
      };
    };
    float raw[3];
  };

  FHSV(){}

  FHSV(float hu, float st, float vl){
    h = hu;
    s = st;
    v = vl;
  }


  bool fromJson(JsonVariant d) {
    bool updated = false;
    if( d.containsKey("h")) {
        h = clamp(d["h"],0.0,360);
        updated = true;
    }
    if( d.containsKey("s")) {
        s = clamp(d["s"]);
        updated = true;
    } 
    if( d.containsKey("v")) {
        v = clamp(d["v"]);
        updated = true;
    }
    if( d.containsKey("h+")) {
        h = wrap(h + d["h+"].as<float>(),360);
        updated = true;
    }
    if( d.containsKey("s+")) {
        s = clamp(s + d["s"].as<float>());
        updated = true;
    } 
    if( d.containsKey("v+")) {
        v = clamp(v + d["v"].as<float>());
        updated = true;
    }
    return updated;
  };

  void toJson(JsonVariant d) {
    d["h"] = h;
    d["s"] = s;
    d["v"] = v;
  }

  void toSerial() {
    Serial.print("HSV  :: h: "); Serial.print(h); Serial.print(", s: "); Serial.print(s); Serial.print(", v: "); Serial.print(v);
    Serial.println();
  }

  // https://www.neltnerlabs.com/saikoled/how-to-convert-from-hsi-to-rgb-white
  void toRGBW(FRGBW& target) {
    float r, g, b, w;
    float cos_h, cos_1047_h;
    float H = (3.14159*wrap(h,360)/(float)180); // Convert to radians.
    float S = clamp(s); // clamp S and I to interval [0,1]
    float I = clamp(v); 
    Serial.println("Converting HSV to RGBW");
    toSerial();
    Serial.print("H: "); Serial.print(H); Serial.print(", S: "); Serial.print(S); Serial.print(", I: "); Serial.print(I);
    Serial.println();
    
    if(H < 2.09439) {
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        r = S*I/3*(1+cos_h/cos_1047_h);
        g = S*I/3*(1+(1-cos_h/cos_1047_h));
        b = 0;
        w = (1-S)*I;
    } else if(H < 4.188787) {
        H = H - 2.09439;
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        g = S*I/3*(1+cos_h/cos_1047_h);
        b = S*I/3*(1+(1-cos_h/cos_1047_h));
        r = 0;
        w = (1-S)*I;
    } else {
        H = H - 4.188787;
        cos_h = cos(H);
        cos_1047_h = cos(1.047196667-H);
        b = S*I/3*(1+cos_h/cos_1047_h);
        r = S*I/3*(1+(1-cos_h/cos_1047_h));
        g = 0;
        w = (1-S)*I;
    }
    target.r = r;
    target.g = g;
    target.b = b;
    target.w = w;
    Serial.print("r: "); Serial.print(r); Serial.print(", g: "); Serial.print(g); Serial.print(", b: "); Serial.print(b); Serial.print(", w: "); Serial.print(w);
    Serial.println();
  }
   
};







struct PointRGBW {
  FRGBW color;
  float position;
  PointRGBW( FRGBW c, float p ) {
    color = c;
    position = p;
  }
};

struct PointARGBW {
  FRGBW color;
  int position;
  PointARGBW( FRGBW c, int p ) {
    color = c;
    position = p;
  }
};

struct AreaRGBW {
  FRGBW color;
  float centre;
  float width;
  AreaRGBW( FRGBW col, float cent, float w)
  {
    color = col;
    centre = cent;
    width = w;
  }

  void fromJson(JsonVariant d) {
    color.fromJson(d);
    if( d.containsKey("centre")) centre = d["centre"];
    if( d.containsKey("width")) width = d["width"];
  };

  void toJson(JsonVariant d) {
    color.toJson(d);
    d["centre"] = centre;
    d["width"] = width;
  };
};

struct AreaARGBW {
  FRGBW color;
  int centre;
  int width;
  /*
  AreaRGBW( FRGBW col, int cent, int w)
  {
  color = col;
  centre = cent;
  width = w;
}
*/
};

#endif
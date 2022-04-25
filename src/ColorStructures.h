#ifndef COLOR_STRUCTURES_H
#define COLOR_STRUCTURES_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Utils.h"
#include "EEPROM.h"
#include "Preferences.h"
#include "LinkedList.h"
//#define DEG_TO_RAD(X) (M_PI*(X)/180)

/**
* This is a float based data container for LED strips
* Absolute waste of memory, but makes some things easier
*/




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

  bool fromJson(JsonVariant d) {
    bool updated = false;
    if( d.containsKey("r")) {
      r = clamp(d["r"]);
      updated = true;
    }
    if( d.containsKey("g")) {
      g = clamp(d["g"]);
      updated = true;
    }
    if( d.containsKey("b")) {
      b = clamp(d["b"]);
      updated = true;
    }
    if( d.containsKey("w")) {
      w = clamp(d["w"]);
      updated = true;
    }
    return updated;
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

  void toEEPROM(Preferences prefs, const char* key) {
    prefs.begin("glow");
    FRGBW intest(0.3,0.4,0.5,0.6);
    prefs.putBytes("glow",&intest,sizeof(FRGBW));
    Serial.print("Saved color to "); Serial.println(key);
    intest.toSerial();
    Serial.print("OK? > "); Serial.println(prefs.isKey(key));
    FRGBW test;
    prefs.getBytes("glow",&test,sizeof(FRGBW));
    Serial.println("Loaded back: ");
    test.toSerial();
  }

  bool fromEEPROM(Preferences prefs, const char* key) {
    //if( ! prefs.isKey(key)) {
      //Serial.print("Couldn't find color in: "); Serial.println(key);
      //return false;
    //}
    prefs.getBytes("glow",this,sizeof(FRGBW));
    return true;
  }
};

static inline FRGBW interpolateRGBW(FRGBW start_col, FRGBW end_col, float amount) {
  return FRGBW(
    start_col.r + (end_col.r - start_col.r)*amount,
    start_col.g + (end_col.g - start_col.g)*amount,
    start_col.b + (end_col.b - start_col.b)*amount,
    start_col.w + (end_col.w - start_col.w)*amount
  );
}


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
      Serial.print("Updating HUE... "); Serial.print(h); Serial.print(" + "); Serial.print(d["h+"].as<float>());Serial.print(" -> ");
        h = wrap(h + d["h+"].as<float>(),360);
        Serial.println(h);
        updated = true;
    }
    if( d.containsKey("s+")) {
      Serial.print("Updating SAT... "); Serial.print(s); Serial.print(" + "); Serial.print(d["s+"].as<float>());Serial.print(" -> ");
        s = clamp(s + d["s+"].as<float>());
        Serial.println(s);
        updated = true;
    } 
    if( d.containsKey("v+")) {
        v = clamp(v + d["v+"].as<float>());
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
    //Serial.println("Converting HSV to RGBW");
    //toSerial();
    //Serial.print("H: "); Serial.print(H); Serial.print(", S: "); Serial.print(S); Serial.print(", I: "); Serial.print(I);
    //Serial.println();
    
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
    //Serial.print("r: "); Serial.print(r); Serial.print(", g: "); Serial.print(g); Serial.print(", b: "); Serial.print(b); Serial.print(", w: "); Serial.print(w);
    //Serial.println();
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

class ColorPoint {
public:
  ColorPoint(float p, FRGBW c) : position(p), color(c) {}
  float position;
  FRGBW color;
};

class InterpolationTable {
public:
  InterpolationTable() {}
  FRGBW getColor(float pos) {
    int size = points.size();
    if( size == 0 ) {
      //Serial.println("No colors...");
      return FRGBW(0,0,0,0);
    }
    //Serial.print("Pos:\t");Serial.print(pos);
    for( int i = 0; i < size; i++ ) {
      ColorPoint* c = points.get(i);
      if(pos < c->position ) {
        if( i == 0 ) { 
          //Serial.println(" < under");
          return c->color; 
        }
        ColorPoint* prev = points.get(i-1);
        //Serial.print(" between ");Serial.print(prev->position);Serial.print("..");Serial.print(c->position);Serial.print(" ==> ");
        float proportion = 
          (pos - prev->position) /  // Distance into interval
          (c->position - prev->position); //Size of interval
        //Serial.println(proportion);
        return interpolateRGBW(prev->color, c->color,proportion);
      }
    }
    //Serial.println(" > over");
    return points.get(size - 1)->color;
  }
  /*
   * Make sure to add points in increasing order
   */ 
  void addPoint(float pos, FRGBW col) {
    points.add(new ColorPoint(pos,col));
  }

  void print() {
    Serial.println("<<");
    for(int i = 0; i < points.size(); i++ ) {
      ColorPoint* c = points.get(i);
      Serial.print(c->position);Serial.print(" => "); c->color.toSerial();
    }
    Serial.println(">>");
  }
protected:
  LinkedList<ColorPoint*> points;
};


#endif
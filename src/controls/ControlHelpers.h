#include "Utils.h"

/*
Used for e.g. a Joystick. It maps like this:
|====minIn======centerIn===maxIn============|
|=minOut====centerOut=================maxOut|

centerIn updates slowly in normal use (joycon drift) but
can be quickly updated at the start

Example JSON:
*/
class CenteringRange {
public:
    void updateCenter(float in, bool fast) {
        float rate = fast ? centerUpdate : centerDrift;
        centerIn = centerIn * (1-rate) + in * rate;
        if( fast ) {
            //Serial.print("> Updating Centre: "); Serial.print(in); Serial.print(" = => "); Serial.println(centerIn);
        }
    }

    void update( float in) {
        updateCenter(in, false);
        if( in > maxIn ) {maxIn = in;}
        if( in < minIn ) {minIn = in;}
        active = abs(in-centerIn) > deadZone;
        if( in >= centerIn ) current = mapF(in, centerIn, maxIn, centerOut, maxOut );
        else current = mapF(in, minIn, centerIn, minOut, centerOut );
        //Serial.print(" >> ");Serial.print(minIn);Serial.print("\t == \t"); Serial.print(centerIn); Serial.print("\t == \t"); Serial.println(maxIn);
        //Serial.print(" << ");Serial.print(minOut);Serial.print("\t == \t"); Serial.print(centerOut); Serial.print("\t == \t"); Serial.println(maxOut);
        //Serial.print(in); Serial.print("\t ==> \t"); Serial.println(current);
    }
    bool isActive() { return active; }
    float getValue() { return current; }


    float centerIn=0.5;
    float minIn=0.0;
    float maxIn=1.0;

    float centerOut=0.0;
    float maxOut=0.5;
    float minOut=-0.5;

    float centerUpdate = 0.05;
    float centerDrift = 0.0001;

    float deadZone=0.1;
protected:
    float current=0.5;
    bool active = false;
};
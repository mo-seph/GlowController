#include "Arduino.h"

struct TimeKeeping {
  long _loopStart;
  long _loopDone;
  long _inputStart;
  long _inputDone;
  long _behavioursStart;
  long _behavioursDone;
  long _showStart;
  long _showDone;
  float _frameRate;
  int logState;
  int logTime;
  long _prevBehavioursStart;
  long outputRate;
  long _lastOutput;
  TimeKeeping() : _frameRate(25), logState(0), logTime(1), _prevBehavioursStart(-1), outputRate(1000), _lastOutput(0) {};

  void loopStart() {
    if( logState ) Serial.println("Starting Loop");
    _loopStart = millis();
  };
  void loopDone() {
    if( logState > 1 ) Serial.println("Finished Loop");
    _loopDone = millis();
  };
  void inputStart() {
    if( logState ) Serial.println("Start Input");
    _inputStart = millis();
  };
  void inputDone() {
    if( logState  > 1) Serial.println("Done Input");
    _inputDone = millis();
  };
  void behavioursStart() {
    if( logState ) Serial.println("Start Behaviours");
    _prevBehavioursStart = _behavioursStart;
    _behavioursStart = millis();
  };
  long delta() {
    if( _prevBehavioursStart < 0 ) return 5;
    else return _behavioursStart - _prevBehavioursStart;
  }
  void behavioursDone() {
    if( logState  > 1) Serial.println("Done Behaviours");
    _behavioursDone = millis();
  };
  void showStart() {
    if( logState ) Serial.println("Start Show");
    _showStart = millis();
  };
  void showDone() {
    if( logState  > 1) Serial.println("Done Show");
    _showDone = millis();
  };
  void setFrameRate(float f) {_frameRate = f;};

  long delayTime() {
    long timeDesired = (long)(1000.0 / _frameRate);
    long timeTaken = _loopDone - _loopStart;
    return  timeDesired - timeTaken;
  };
  void delayIfNeeded() {
    long d = delayTime();
    if( d > 0 && d < 1000 ) {
      delay(d);
    }
  }


  void printOutput() {
    if( millis() < _lastOutput + outputRate ) {
      return;
    }
    _lastOutput = millis();
    if( logTime ) {
      long update_t = _loopDone - _loopStart;
      Serial.print("Update time: ");Serial.print(update_t);
      Serial.print(" FPS: ");Serial.println((int)(1000.0/update_t));
    }
    if( logTime > 1 ) {
      Serial.print("Delta: ");Serial.print(_loopDone - _loopStart);
      Serial.print(" Input time: ");Serial.print(_inputDone - _inputStart);
      Serial.print(" Behaviour time: ");Serial.print(_behavioursDone - _behavioursStart);
      Serial.print(" Show time: ");Serial.print(_showDone - _showStart);
      Serial.print(" Delay time: ");Serial.print(delayTime());
      Serial.println();
    }
  }
};

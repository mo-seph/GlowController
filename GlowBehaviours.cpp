#include "GlowBehaviours.h"

GlowBehaviour::GlowBehaviour(GlowController *c,const char *type) :
  controller(c), strip(c->getStrip()), type(type) { }
//void set_strip(GlowStrip *s){ strip = s;}

/*
virtual void init(JsonVariant d) {
  if( d.containsKey("name")) setName( d["time"] );
  stateFromJson(d);
};

virtual ~GlowBehaviour() {};


virtual void stateFromJson(JsonVariant d) {};
virtual void stateToJson(JsonVariant d) {}

virtual void update(long millis) { if(active) doUpdate(millis); };
virtual void doUpdate(long millis) {};

virtual const char* getName() {return name;};
virtual void setName(const char* n) {strcpy(name,n);};


void setActive(bool act) {
  if( ! active && act ) { active = true; }
  else if( active && !act ) { active = false; }
}
bool isActive() { return active; };
virtual const char* getType() {return type;};

*/

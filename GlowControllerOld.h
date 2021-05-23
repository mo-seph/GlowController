/*
  Library for creating a device that 'runs' LED strips. It takes input from
  a serial (or other?) connection as a pile of bytes, and maps that to a
  connected LED strip.

  We assume running on fancy hardware that doesn't mind some floating point,
  assuming that communication (both serial input and LED output) dominate,
  and that memory is also plentiful...

  Also assuming that we (potentially) have a white channel...

  The library has a collection of behaviours that can be run to do different
  things. It expects three kinds of packet:
  - setup packets configure strip functions etc.
  - behaviour packets choose a behaviour and specify parameters
  - data packets are used to update
*/

#ifndef GlowControllerO_h
#define GlowControllerO_h
#include "Arduino.h"
#include "GlowBehaviours.h"
#include "GlowStrip.h"
#include <OctoWS2811.h>

// Packet types
#define PACKET_SETUP 1
#define PACKET_BEHAVIOUR 2
#define PACKET_DATA 3

// Behaviour IDs
#define BEHAVIOUR_FILL 1
#define BEHAVIOUR_FILL_HSV 2
#define BEHAVIOUR_BREATHE 3

// Setup codes
#define BRIGHTNESS 1



class GlowControllerOld {
public:
  GlowController(int id);
  void initialise(GlowStrip* s);
  void update_data(const uint8_t* buffer, size_t size);
  void update();
protected:
  GlowStrip *strip;
  GlowBehaviour* behaviours[255];
  void handle_setup(const uint8_t* buffer, size_t size);
  void handle_behaviour(const uint8_t* buffer, size_t size);
  void handle_data(const uint8_t* buffer, size_t size);
  unsigned long last_update = 0;
  GlowBehaviour* current_behaviour;
};



#endif

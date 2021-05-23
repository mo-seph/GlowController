#include "GlowController.h"
#include "GlowBehaviours.h"
#include "Arduino.h"

/*
void print_buffer(const uint8_t* buffer, size_t size) {
  int num = min((uint)size,(uint)100);
  Serial.print("Got ");
  Serial.print(size);
  Serial.print(" showing first ");
  Serial.println(num);
  for( uint i = 0; i < size; i++ ) {
    Serial.print(buffer[i]);
    Serial.print(" ");
  }
  Serial.println();
}

GlowController::GlowController(int id): behaviours() {

}

void GlowController::initialise(GlowStrip *s) {
  strip = s;

  Serial.println("Runner initialising");
  //Serial.println(id);
  behaviours[BEHAVIOUR_FILL] = new Fill();
  //behaviours[BEHAVIOUR_FILL_HSV] = new FillHSV();
  behaviours[BEHAVIOUR_BREATHE] = new Breathe();
  behaviours[4] = new Glow();
  Serial.println("Runner done initialising");

  Serial.println("Runner setting strip");
  for( uint i = 0; i <= 254;i++ ) {
    if( behaviours[i] ) behaviours[i]->set_strip(strip);
  }
  Serial.println("Runner done setting strip");
}

void GlowController::update_data(const uint8_t* buffer, size_t size) {
  if( size == 0 ) return;
  switch(buffer[0]) {
    case PACKET_SETUP:
      handle_setup(buffer+sizeof(uint8_t),size-1);
      break;
    case PACKET_BEHAVIOUR:
      handle_behaviour(buffer+sizeof(uint8_t),size-1);
      break;
    case PACKET_DATA:
      handle_data(buffer+sizeof(uint8_t),size-1);
      break;
  }
}

void GlowController::update() {
  unsigned long current = millis();
  long delta = 5;
  if(last_update != 0 ) delta = current - last_update;
  //Serial.print("Updating afer millis: ");
  //Serial.println(delta);
  if( current_behaviour ) current_behaviour->update(delta);
}

void GlowController::handle_setup(const uint8_t* buffer, size_t size) {
  Serial.println("Setup packet: ");
  print_buffer(buffer,size);
  switch(buffer[0]) {
    case BRIGHTNESS:
      uint8_t bright = buffer[1];
      Serial.print("Setting brightness to: ");Serial.println(bright);
      strip->brightness = ((float)bright) / 256.0;
      strip->show();
      break;
  }
}


void GlowController::handle_behaviour(const uint8_t* buffer, size_t size) {
  Serial.println("Behaviour packet: ");
  print_buffer(buffer,size);
  int behaviour_number = buffer[0];
  current_behaviour = behaviours[behaviour_number];
  if( current_behaviour ) {
    Serial.print("Starting behaviour number ");
    Serial.print(behaviour_number);
    Serial.print(": ");
    Serial.println(current_behaviour->getName());
    current_behaviour->init(buffer+sizeof(uint8_t),size-1);
  }

}
void GlowController::handle_data(const uint8_t* buffer, size_t size) {
  Serial.println("Data packet: ");
  print_buffer(buffer,size);
  if(current_behaviour) current_behaviour->update_data(buffer+sizeof(uint8_t),size-1);

}
 */

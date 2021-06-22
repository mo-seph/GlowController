# Glow Strip

This library abstracts away from underlying communications libraries for
working with addressable LEDs. Why? There's several out there, and I've had to
try several in a row for making long RGBW strips work. This library:
- is completely wasteful about memory and floating point operations
- allows for different strip libraries to be used easily, currently
  - https://github.com/FastLED/FastLED (with experimental RGBW support)
  but also looking at:
  - https://www.pjrc.com/teensy/td_libs_OctoWS2811.html (needs RGBW support)
  - https://github.com/Makuna/NeoPixelBus

# Organisation

Create a GlowStrip, using one of the existing communication backends. It needs
an array of floats 4 * the length of the strip.

It supports simple commands:
- init
- setPixel(r,g,b,w); setPixel(r,g,b); setPixel(w)
- fill

It might support more complex commands:
- glow (to make/add a smooth glow over part of the strip)
- spatial interpolation
- clearRGB
- clearWhite

And some thing that can add to the existing values, rather than setting them

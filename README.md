# Glow controller
This library makes a semi-autonomous LED light based on RGBW strip. It is
targetted at modern micro boards like ESP32s, that have FPUs and plenty of memory,
and ideally WiFi connections. As such, it tries to make things easy, using floats
and JSON for specifying everything, at the expense of more RAM and compute.

Key features:
- Layer multiple `Behaviour`s on top of each other, to have e.g. a background colour, an intermittent alarm, something that responds to bluetooth presence etc.
- Connectivity is provided through `Features` and `Connectors` that allow for different configurations of websockets, MQTT, physical buttons, network time, OTA update etc.
- All of the colours use `floats` to make for easy fading etc.
- Uses different LED libraries for the backend - currently tested with a modified FastLED library that supports RGBW strip.

# Setup

The strip is setup using a JSON document - see example in Platformio project. This defines the initial set of `Behaviour`s that the strip has, as well as its name, id and other important variables.

# Behaviours

`Behaviour`s can be seen in `GlowBehaviours.h` and related files. The controller has a list of behaviours, and goes through them one by one each update, drawing them onto the strip.

## Current Behaviours
- `Fill` has a color, and fills the whole strip with that color. It interpolates over time, so if a new color comes in, it will gradually fade to that color over a number of seconds.
- `Watchdog` helps to make sure the strip has not crashed. It has two pixels showing the speed of the last frame, and the worst speed from the last 30 frames (cyan = <10ms, green=<30ms, fades to red up to 100ms, magenta = > 100ms), and then a single pixel that moves along every 10 frames or something like that
- `Glow` shows a diffuse glow, parameterised by a centre point, a width and a color. The glow can also move along at a set rate.
- `PixelClock` shows the current time with pixels, The first group is one per hour, the second is one per 5 minutes, the last is one per 5 seconds.
- `Alarms` is a countdown timer - it shows a gradually shrinking bar for a given time. When it hits zero, the given area flashes.

## Create new Behaviours
Each behaviour should implement:
- `doUpdate(long millis)` is the main method where the behaviour updates any internal state and writes its output to the strip
- `stateToJson(JsonVariant)` and `stateFromJson(JsonVariant)` write the behaviours state to a json document or update it from one respectively.

A JSON state might be something like:
```
{
  "name":"Background",
  "active":true,
  "data": {
    "r": 0.1,
    "g": 0.3,
    "b": 0.7,
    "w": 0.3
  }
}
```

The `init` method gets this whole thing. The `stateToJson` and `stateFromJson` methods just get the `"data"` section to work with.

Other methods:
- `init(JsonVariant)` sets the initial parameters for the `Behaviour`, based on a Json object passed in. This mostly just sets the name and activation for the behaviour, and passes the data on to `stateFromJson`, so shouldn't need overriding.


# Features and Connectors

`Feature`s are background options, that have some kind of setup and then provide functionality. Current features are:
- `WiFiFeature` connects the strip to a wifi network, and also enables OTA updates (although this should probably be optional!)
- `NTPFeature` enables getting the current time from an NTP server

`Connector`s also get run every frame, but have additional code for sending and updating state of the strip.

Current connectors:
- `SerialConnector` outputs the current state over the serial port
- `MQTT` connects the strip to an MQTT server to send and receive commands
- websockets is on the way...

## MQTT Connector
- Find out which devices are there: send anything to `leds/ping`
- Listen for updates from a devices on `leds/<id>/state`
- Ask devices to do something on `leds/<id>/commands`

Note: probably needs to get split up a bit so that each behaiour can update
individually

Example:
```
mosquitto_pub -h "192.168.178.123" -t "leds/ping" -m '{"ping":1}'
```
Provokes ping response on `leds/ping`:
```
{"id":"living","name":"Living Room Lights"}
```

Then:
```
mosquitto_pub -h "192.168.178.123" -t "leds/living/commands" -m '{"state":1}'
```

Provkes state update:
```
{
   "id":"living",
   "state":[
      {
         "active":true,
         "type":"Fill",
         "name":"Background",
         "id":0
      },
      {
         "active":true,
         "type":"Alarms",
         "name":"Alarms",
         "id":6
      },
      {
         "active":false,
         "type":"Watchdog",
         "name":"�",
         "id":16
      }
   ]
}
```


# Roadmap
- [ ] Overall brightness control/limiting for the strip
- [ ] Brightness patterns to avoid wasting light when occluded?
- [ ] Brightness curves
- [ ] Dithering/noise?
- [X] Check in about saving base color to flash
- [X] Buttons input
- [ ] Turn on/off in setup?
- [ ] Save current JSON state to flash?
- [ ] Add new behaviours via JSON?
- [X] Load config from filesystem
- [ ] New behaviours:
   - [ ] Fire
   - [ ] Dawn

# Reworking/Bugfixing
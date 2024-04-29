# Controls

The control system has a few different elements

ControlElement is supposed to be a physical control (e.g. a knob or a button)
ControlAction is a specification for what that thing does (e.g. send a command)

# Actions

Targets a particular behaviour, and calls a command on it:
```
   {
          "type":"external",
          "behaviour_id":0,
          "target":"living",
          "path":["data","h+"]
    }
```
Fields:
- `type`: internal or external
- `behaviour_id`: the behaviour to send the command to
- `path`: the command to call on the given behaviour
- `target`: for an external action, the name of the device to send commands to

## Example Paths
- `["data","h+"]` - increases the hue by the given amount. Can also do `s+` for sat and `v+` for brightness. Looks like `h` has a big range, while `s` and `v` are `0..1`
- `["active"]` - boolean, on or off for the behaviour
# Elements

Elements take some things that define their physical connections (e.g. pins for knobs)
and part of their setup (e.g. max/min values, dead zones etc.). They then take a bunch of
Actions for what they should do.


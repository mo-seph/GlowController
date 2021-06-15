# Glow controller


## MQTT Mesesages
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

# Setting up MQTT


MQTT support seems a bit fiddly - needs websockets to work with React apps.

With mosquitto, you can enable websocket support by adding these lines to
/opt/local/etc/mosquitto/mosquitto.conf (or /etc/mosquitto/mosquitto.conf depending on your system):
`
listener 9001
protocol websockets

listener 1883
protocol mqtt
`
Then on the browser side, you can use the MQTT library: https://github.com/mqttjs/MQTT.js/#browser

- example test command:
`
mosquitto_pub -t leds/commands -h localhost -m '{"update":0, "data":{"r":0.0,"g":0.9,"b":0.0,"w":0.0}}'
`

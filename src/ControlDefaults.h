#ifndef NUM_LEDS
#define NUM_LEDS 100
#endif

#ifndef LED_DATA_PIN
#define LED_DATA_PIN 12
#endif

//#define STATE_LOGGING 1
//#define TIME_LOGGING 1

/* WIFI Defs */
#ifndef WIFI_SSID
#define WIFI_SSID MyWifi SSID Thingy
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD MyPassword
#endif

/* MQTT Defs */
#ifndef MQTT_SERVER
#define MQTT_SERVER 192.168.178.108
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif

/* NTP Defs */
#ifndef NTP_SERVER
#define NTP_SERVER pool.ntp.org 
#endif

#ifndef GMT_OFFSET
#define GMT_OFFSET 1
#endif

#ifndef DAYLIGHT_OFFSET
#define DAYLIGHT_OFFSET 1
#endif

//const char* id = "living";
//const char* name = "Living Room Lights";

#define str(x) #x
#define strname(name) str(name)

#ifndef DEVICE_ID
#define DEVICE_ID test
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME Test Device
#endif
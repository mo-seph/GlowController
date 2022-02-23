#include <PacketSerial.h>
#include <Adafruit_NeoPixel.h>
#include <LEDRunner.h>

PacketSerial myPacketSerial;

#define LED_PIN    2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 900

#define DEFAULT_BRIGHTNESS 200

#define DEFAULT_LEVEL 200

#define MIN_UPDATE 5

#define START_RATE 0

uint32_t color = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
LEDRunner runner(5);
long last_millis = 0;

void setup() {
  //Serial.begin(9600);
  Serial.println("Starting up");

  Serial.println("Starting Strip");
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(DEFAULT_BRIGHTNESS);
  for( int i = 0; i < 255; i += 10 ) {
    strip.fill(strip.Color(0,0,0,i));
    strip.show();
    delay(START_RATE);
  }
  for( int i = 255; i > 0; i -= 10 ) {
    strip.fill(strip.Color(0,0,0,i));
    strip.show();
    delay(START_RATE);
  }
  Serial.println("Strip Done");
  
  for( int i = 0; i < DEFAULT_LEVEL; i+= 10 ) {
    strip.fill(strip.Color(0,0,0,i));
    strip.show();            // Turn OFF all pixels ASAP
    delay(START_RATE);
  }
  Serial.println("Starting Runner");
  runner.initialise(&strip);

  Serial.println("Opening Communications");
  Serial1.begin(115200);
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler(&onPacketReceived);


}

void loop() {
  myPacketSerial.update();
  long current = millis();
  if( current > last_millis + MIN_UPDATE ) {
    runner.update();
    last_millis = current;
  }
  delay(3);
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  Serial.print("Teensy got ");
  Serial.print(size);
  Serial.println(" bytes of data");
  runner.update_data(buffer,size);
}

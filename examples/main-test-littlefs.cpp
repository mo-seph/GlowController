#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>



void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(100); }
    Serial.print("Starting up. Beginning littlefs...");
    if (!LittleFS.begin(true)) {
        Serial.println("Couldn't start LittleFS?");
    };
    Serial.println(" Done!");

    Serial.print("Total: "); Serial.print(LittleFS.totalBytes()); Serial.print(", Used: "); Serial.println(LittleFS.usedBytes());

    File file = LittleFS.open("/conf.json", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("File Content:");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void loop()
{
}

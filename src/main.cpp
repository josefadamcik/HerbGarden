#include <Arduino.h>

#include "config.h"
#include "keys.h"
#include <WEMOS_SHT3X.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightSensor;
SHT3X sht30(0x45);

// void otaInitialize() {
//     // Initialise OTA in case there is a software upgrade
//     ArduinoOTA.setHostname(HOSTNAME);
//     ArduinoOTA.onStart([]() { Serial.println("Start"); });
//     ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
//     ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//         Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//     });
//     ArduinoOTA.onError([](ota_error_t error) {
//         Serial.printf("Error[%u]: ", error);
//         if (error == OTA_AUTH_ERROR) {
//             Serial.println("Auth Failed");
//         } else if (error == OTA_BEGIN_ERROR) {
//             Serial.println("Begin Failed");
//         } else if (error == OTA_CONNECT_ERROR) {
//             Serial.println("Connect Failed");
//         } else if (error == OTA_RECEIVE_ERROR) {
//             Serial.println("Receive Failed");
//         } else if (error == OTA_END_ERROR) {
//             Serial.println("End Failed");
//         }
//     });

//     ArduinoOTA.begin();
// }

void setup() { 
    Serial.begin(115200); 
    Wire.begin();
    lightSensor.begin();
}

void loop() {
    if (sht30.get() == 0) {
        Serial.print("Temperature in Celsius : ");
        Serial.println(sht30.cTemp);
        Serial.print("Relative Humidity : ");
        Serial.println(sht30.humidity);
        Serial.println();
    } else {
        Serial.println("Error!");
    }
    float lux = lightSensor.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
    delay(1000);
}

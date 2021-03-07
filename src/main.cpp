#include <Arduino.h>
#include <BH1750.h>
#include <WEMOS_SHT3X.h>
#include <Wire.h>
#include <mqtt_client.h>

#include "WiFi.h"
#include "config.h"
#include "keys.h"

#define LED_RED_PIN 33
#define LED_GREEN_PIN 32

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


void ledOkState(bool on) {
    digitalWrite(LED_GREEN_PIN, on);
}

void ledErrState(bool on) {
    digitalWrite(LED_RED_PIN, on);
}


void ledState(bool ok) {
    ledOkState(ok);
    ledErrState(!ok);
}

void connectToWifi() {
    if (WiFi.status() != WL_CONNECTED) {
        ledOkState(false);
        ledErrState(false);
        WiFi.begin(MYSSID, MYPASS);
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Connecting to WiFi..");
            delay(250);
            ledOkState(true);
            delay(250);
            ledOkState(false);

            if (WiFi.status() == WL_CONNECT_FAILED) {
                Serial.println("Wifi connection failed");
                break;
            }
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        ledOkState(true);
    } else {
        ledState(false);
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    lightSensor.begin();

    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    ledState(true);
}

void loop() {
    connectToWifi();
    if (sht30.get() == 0) {
        Serial.print("Temperature in Celsius : ");
        Serial.println(sht30.cTemp);
        Serial.print("Relative Humidity : ");
        Serial.println(sht30.humidity);
    } else {
        ledState(false);
        Serial.println("Error!");
    }

    float lux = lightSensor.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println("lx");

    Serial.println();

    delay(1000);
}

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <BH1750.h>
#include <WEMOS_SHT3X.h>
#include <mqtt_client.h>

#include "config.h"
#include "keys.h"

#define LED_RED_PIN 33
#define LED_GREEN_PIN 32

BH1750 lightSensor;
SHT3X sht30(0x45);

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

void onWifiConnected() {
    Serial.println("Wifi connected.");
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());
    Serial.println();
    ArduinoOTA.begin();
}

void connectToWifi() {
    bool connectionAttempt = false;
    if (WiFi.status() != WL_CONNECTED) {
        connectionAttempt = true;
        ledOkState(false);
        ledErrState(false);
        WiFi.config(ip, gateway, subnet, gateway, gateway);
        WiFi.setHostname(HOSTNAME);
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
        if (connectionAttempt) {
            onWifiConnected();
        }
    } else {
        ledState(false);
    }
}

void setupOta() {
     // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(HOSTNAME);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    lightSensor.begin();

    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    ledState(true);
    setupOta();
}

void measure() {
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
}

void loop() {
    connectToWifi();
    ArduinoOTA.handle();
    measure();
    delay(1000);
}

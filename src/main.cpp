#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <BH1750.h>
#include <WEMOS_SHT3X.h>
#include "mqtt.h"

#include "config.h"
#include "keys.h"

#define LED_RED_PIN 33
#define LED_GREEN_PIN 32

BH1750 lightSensor;
SHT3X sht30(0x45);

esp_mqtt_client_handle_t mqttClient;
bool mqttConnected = false;

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

typedef struct {
  float temperature;
  float humidity;
  float light;
  unsigned long lastUpdate;
  bool error;
} Measurements;

static void onMqttEvent(esp_mqtt_event_handle_t event) {
    // int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            Serial.println("MQTT connected");
            mqttConnected = true;
            break;
        case MQTT_EVENT_DISCONNECTED:
            Serial.println("MQTT disconnected");
            mqttConnected = false;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // Serial.print("MQTT subscribed, msg_id");
            // Serial.print(event->msg_id);
            // Serial.println();
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            // Serial.print("MQTT unsubscribed, msg_id");
            // Serial.print(event->msg_id);
            // Serial.println();
            break;
        case MQTT_EVENT_PUBLISHED:
            Serial.print("MQTT published, msg_id");
            Serial.print(event->msg_id);
            Serial.println();
            break;
        case MQTT_EVENT_DATA:
            Serial.print("MQTT published, msg_id");
            Serial.print(event->msg_id);
            Serial.print(event->topic);
            Serial.print(":");
            Serial.print(event->data);
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            Serial.println("MQTT error");
            mqttConnected = false;
            ledState(false);
            // if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            //     // log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            //     // log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            //     // log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            //     // ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            // }
            break;
        default:
            break;
    }
}

static void mqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    onMqttEvent((esp_mqtt_event_handle_t)event_data);
}

void onWifiConnected() {
    Serial.println("Wifi connected.");
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());
    Serial.println();
    ArduinoOTA.begin();
    mqttClient = setupMqtt(MQTT_BROKER, MQTT_CLIENT, mqttEventHandler);
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

Measurements measure() {
    Measurements measurements;
    measurements.error = false;
    if (sht30.get() == 0) {
        measurements.temperature = sht30.cTemp;
        measurements.humidity = sht30.humidity;
    } else {
        Serial.println("SHT error");
        measurements.error = true;
    }
    float light = lightSensor.readLightLevel();
    if (light < 0.0) {
        Serial.println("light error");
        measurements.error = true;
    } else {
        measurements.light = light;
    }
     
    return measurements;
}

void printMeasurements(Measurements measurements) {
    Serial.print("Temperature [Celsius]: ");
    Serial.println(measurements.temperature);
    Serial.print(" Relative Humidity: ");
    Serial.println(measurements.humidity);
    Serial.print(" Light [lx]: ");
    Serial.print(measurements.light);
    Serial.println();
}

void publishValue(const char* topic, float value) {
    char tmpStr[8]; // Buffer big enough for 7-character float
    dtostrf(value, 4, 2, tmpStr);
    int msgId = esp_mqtt_client_publish(mqttClient, topic, tmpStr, /*len*/0, /*qos*/ 1,  /*retain*/0);
    Serial.print("Publish ");
    Serial.print(msgId);
    Serial.println();
}

void publishMeasurements(Measurements measurements) {
    publishValue(MQTT_TOPIC_PREFIX "temperature", measurements.temperature);
    publishValue(MQTT_TOPIC_PREFIX "humidity", measurements.humidity);
    publishValue(MQTT_TOPIC_PREFIX "light", measurements.light);
}


void loop() {
    static Measurements lastMeasurements;
    connectToWifi();
    ArduinoOTA.handle();
    if (mqttConnected) {
        unsigned long now = millis();
        if (lastMeasurements.lastUpdate == 0 
            || now - lastMeasurements.lastUpdate >= measurementPeriodSec * 1000) {
            lastMeasurements = measure();
            lastMeasurements.lastUpdate = now;
            if (lastMeasurements.error) {
                Serial.println("Error!");
                ledState(false);
                //TODO: error MQTT topic
            } else {
                ledState(true);
                printMeasurements(lastMeasurements);
                publishMeasurements(lastMeasurements);
            }
        }
    } else {
        Serial.println("MQTT not connected");
        ledState(false);
    }
    delay(1000);
}

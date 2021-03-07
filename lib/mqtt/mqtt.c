#include <mqtt.h>

#include <Arduino.h>

esp_mqtt_client_handle_t setupMqtt(const char *mqqtUri, const char* clientId, esp_event_handler_t eventHandler) {
    const esp_mqtt_client_config_t mqttConfig = {
        .uri = mqqtUri,
        .client_id = clientId

    };
    esp_mqtt_client_handle_t mqttClient = esp_mqtt_client_init(&mqttConfig);

    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_ANY, eventHandler, mqttClient);
    esp_mqtt_client_start(mqttClient);
    return mqttClient;
}

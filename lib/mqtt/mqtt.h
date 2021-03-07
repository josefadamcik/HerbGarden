#include <mqtt_client.h>

#ifndef mqtt_h
#define mqtt_h


#ifdef __cplusplus
extern "C" {
#endif

esp_mqtt_client_handle_t setupMqtt(const char *mqqtUri, const char* clientId, esp_event_handler_t event_handler);

#ifdef __cplusplus
}
#endif
#endif
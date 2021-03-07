#define HOSTNAME "herbgarden.local"
#define MQTT_BROKER "mqtt://192.168.178.58:1883"
#define MQTT_TOPIC_PREFIX  "home/herbgarden/"
#define MQTT_CLIENT "herbgarden"

const unsigned long measurementPeriodSec = 60;

IPAddress ip(192, 168, 178, 69);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
# RoomMonitor

Firmware for DIY indoor herb garden.

- ESP32 wifi connection & brain
- SHT30 - temperature and humidity sensor
- BH1750 - light intensity sensor.

All sesnors communicate via I2C bus. Data are sent to a MQTT broker.

PlatformIO is used to build this project but it should be easy to convert it back to a project for Arduino IDE.

## Config



- change the name of the room in config.h
- setup IP and port for MQTT server in main.cpp

```
const char mqttServer[] = "192.168.178.31";
const int mqttServerport = 1883; 
```

- setup IP for the device and network details in main.cpp

```
IPAddress ip(192, 168, 178, 51);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
```

### OTA

TODO: update

You need to know IP of the device.

Setup in `platformio.ini` as this:

```
platform = espressif8266
board = esp12e
framework = arduino
upload_protocol = espota
upload_port = 192.168.178.51
```

- Restart the device and press shortly PRG after restart. The device will switch into "waiting for OTA" mode.
- Run upload through platformio.


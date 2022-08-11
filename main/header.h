#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define MQTT_PUB_TEMP "esp32/dht/temperature"
#define MQTT_PUB_HUM  "esp32/dht/humidity"
#define MQTT_PUB_SOIL_HUM  "esp32/soil_humidity"


#define DHT11_PIN1 27
#define DHT11_PIN2 4
#define DHTTYPE DHT11
#define AIR_SENSOR 1
#define SOIL_HUMIDITY_LIMIT 300
#define SOIL_SENSOR_LIMIT 4095
#define SENSOR_SOIL 33


void sensorManager(DHT **sensorList, AsyncMqttClient *mqttClient);

void wifiScan();

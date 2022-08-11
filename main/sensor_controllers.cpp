#include <Arduino.h>
#include "header.h"

void temperatureController(DHT* PIN, int Sensor, AsyncMqttClient *mqttClient) {
      double dTemp = PIN[Sensor].readTemperature();
      if (isnan(dTemp)) {
        Serial.println("Failed to read from DHT sensor!");
        return ;
      }
      
      Serial.print("Temperature ");
      Serial.print(" : ");
      Serial.print(dTemp);
      Serial.println("°C");

      
      // Publish an MQTT message on topic esp32/dht/temperature
      uint16_t packetIdPub1 = mqttClient->publish(MQTT_PUB_TEMP, 1, true, String(dTemp).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_TEMP, packetIdPub1);
      Serial.printf("Message: %.2f \n", dTemp);
}

void humidityController(DHT* PIN, int Sensor, AsyncMqttClient *mqttClient) {
    double dHumi = PIN[Sensor].readHumidity();
    if (isnan(dHumi)) {
        Serial.println("Failed to read from DHT sensor!");
        return ;
    }
    
    Serial.print("Humidity ");
    Serial.print(Sensor);
    Serial.print(" : ");
    Serial.print(dHumi);
    Serial.println("%");

    // Publish an MQTT message on topic esp32/dht/humidity
    uint16_t packetIdPub2 = mqttClient->publish(MQTT_PUB_HUM, 1, true, String(dHumi).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM, packetIdPub2);
    Serial.printf("Message: %.2f \n", dHumi);
     
}

void checkSoilMoisture(AsyncMqttClient *mqttClient){
  double averageValue = 0; 
    for (int limit = 300; limit; limit--) {
      averageValue += analogRead(SENSOR_SOIL);
    }
    double sensorValue = averageValue / 300;
    double soilHumidity = ((SOIL_SENSOR_LIMIT - sensorValue) * 100) / SOIL_SENSOR_LIMIT;
    //double soilHumidity = sensorValue;
    Serial.print("Soil humidity : ");
    Serial.print(soilHumidity);
    Serial.print("%\n");
    
    uint16_t packetIdPub3 = mqttClient->publish(MQTT_PUB_SOIL_HUM, 1, true, String(soilHumidity).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_SOIL_HUM, packetIdPub3);
    Serial.printf("Message: %.2f \n", soilHumidity);
    
    if (sensorValue < SOIL_HUMIDITY_LIMIT)
      Serial.print("Ton pot est bien hydraté :)\n");
    else
      Serial.print("Ton pot a soif :p\n");  
}

void sensorManager(DHT **sensorList, AsyncMqttClient *mqttClient){
    DHT *PIN = *sensorList;

    for (int i = 0; i < AIR_SENSOR; i++) {
        Serial.print("Sensor ");
        Serial.println(i);
        humidityController(PIN, i, mqttClient);
        temperatureController(PIN, i, mqttClient);
    }

    checkSoilMoisture(mqttClient);


    Serial.println("______________________________________\n");
}

#include "header.h"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(85, 170, 140, 88)
#define MQTT_PORT 1883


//**** Air sensor
DHT dht1(DHT11_PIN1, DHTTYPE);
//DHT dht2(DHT11_PIN2, DHTTYPE);

DHT *PIN[AIR_SENSOR] = {&dht1};


//********* Wifi connection

const char* ssid = "iPhone de Samuel";
const char* password = "ilestbolelavabo";


WiFiClient espClient;
PubSubClient client(espClient);


AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings


void connectToMqtt() {
  Serial.println("Connecting to MQTT…");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}


void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void connectToWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  Serial.print(WiFi.status());
  while (WiFi.status() != WL_CONNECTED) {
      
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//******* Before Main Loop
void setup()
{
  Serial.begin(115200);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    //mqttClient.onSubscribe(onMqttSubscribe);
    //mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    // If your broker requires authentication (username and password), set them below
    mqttClient.setCredentials("bryan", "test1234");
    connectToWifi();  
  //    wifiScan();

    //** Start les capteurs air 
    for (int sensor = 0; sensor < AIR_SENSOR; sensor++) {
      PIN[sensor]->begin();
    }

  Serial.println("Poto-roazh");
  Serial.println("____________");
}

//****** Main Loop
void loop()
{
    sensorManager(PIN, &mqttClient);
    delay(8000);
}

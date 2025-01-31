// Include necessary libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// MQTT Broker
const char* mqtt_server = "mosquitto";
const int mqtt_port = 1883;
const char* mqtt_user = "your_MQTT_USER";
const char* mqtt_password = "your_MQTT_PASSWORD";

// LoRa settings
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26
#define BAND 915E6

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Send command over LoRa
  LoRa.beginPacket();
  LoRa.print(messageTemp);
  LoRa.endPacket();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp32/commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Initialize LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check for LoRa messages
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    Serial.print("LoRa Received: ");
    Serial.println(incoming);

    // Parse incoming data
    int firstComma = incoming.indexOf(',');
    String clientName = incoming.substring(0, firstComma);
    String data = incoming.substring(firstComma + 1);

    // Determine sensor type
    int sensorIndex = data.indexOf("Sensor:");
    String sensorType = data.substring(sensorIndex + 7, data.indexOf(',', sensorIndex));

    if (sensorType == "THC-S") {
      processTHCSensorData(clientName, data);
    } else if (sensorType == "BME280") {
      processBME280SensorData(clientName, data);
    } else {
      Serial.println("Unknown sensor type");
    }
}

void processTHCSensorData(String clientName, String data) {
    // Parse THC-S sensor data
    int tempIndex = data.indexOf("Temp:");
    int moistureIndex = data.indexOf("Moisture:");
    int conductivityIndex = data.indexOf("Conductivity:");

    String temperature = data.substring(tempIndex + 5, moistureIndex - 1);
    String moisture = data.substring(moistureIndex + 9, conductivityIndex - 1);
    String conductivity = data.substring(conductivityIndex + 13);

    // Publish to individual MQTT topics under sensor type
    client.publish(("esp32/" + clientName + "/THC-S/temperature").c_str(), temperature.c_str());
    client.publish(("esp32/" + clientName + "/THC-S/moisture").c_str(), moisture.c_str());
    client.publish(("esp32/" + clientName + "/THC-S/conductivity").c_str(), conductivity.c_str());
}

void processBME280SensorData(String clientName, String data) {
    // Parse BME280 sensor data
    int tempIndex = data.indexOf("Temp:");
    int humidityIndex = data.indexOf("Humidity:");
    int pressureIndex = data.indexOf("Pressure:");

    String temperature = data.substring(tempIndex + 5, humidityIndex - 1);
    String humidity = data.substring(humidityIndex + 9, pressureIndex - 1);
    String pressure = data.substring(pressureIndex + 9);

    // Publish to individual MQTT topics under sensor type
    client.publish(("esp32/" + clientName + "/BME280/temperature").c_str(), temperature.c_str());
    client.publish(("esp32/" + clientName + "/BME280/humidity").c_str(), humidity.c_str());
    client.publish(("esp32/" + clientName + "/BME280/pressure").c_str(), pressure.c_str());
}

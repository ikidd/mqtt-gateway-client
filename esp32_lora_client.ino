#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// LoRa settings
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26
#define BAND 915E6

// BME280 settings
Adafruit_BME280 bme; // I2C

unsigned long lastSendTime = 0;
const unsigned long interval = 30000; // 30 seconds

// Client name
String clientName = "Client1";

void setup() {
  Serial.begin(115200);

  // Initialize LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");

  // Initialize BME280
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  // Check for LoRa messages
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    Serial.print("Received command: ");
    Serial.println(incoming);

    // Process command and send a response
    String response = "Command received: " + incoming;
    LoRa.beginPacket();
    LoRa.print(response);
    LoRa.endPacket();
  }

  // Send BME280 data every 30 seconds
  if (millis() - lastSendTime > interval) {
    lastSendTime = millis();
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    String data = clientName + ",Temp:" + String(temperature) + ",Humidity:" + String(humidity) + ",Pressure:" + String(pressure);
    Serial.println("Sending BME280 data: " + data);

    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
  }
}

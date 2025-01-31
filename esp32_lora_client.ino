#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ModbusMaster.h>

// LoRa settings
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26
#define BAND 915E6

ModbusMaster node;

// RS485 settings
#define RS485_RX 16
#define RS485_TX 17
#define RS485_BAUD 9600

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
    // Initialize RS485
  Serial2.begin(RS485_BAUD, SERIAL_8N1, RS485_RX, RS485_TX);
  node.begin(1, Serial2); // 1 is the Modbus slave ID
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

  // Send sensor data every 30 seconds
  if (millis() - lastSendTime > interval) {
    lastSendTime = millis();

    String thcData = readTHCSensor();
    String bmeData = readBMESensor();

    if (!thcData.isEmpty()) {
      Serial.println("Sending THC-S sensor data: " + thcData);
      LoRa.beginPacket();
      LoRa.print(thcData);
      LoRa.endPacket();
    }

    if (!bmeData.isEmpty()) {
      Serial.println("Sending BME280 sensor data: " + bmeData);
      LoRa.beginPacket();
      LoRa.print(bmeData);
      LoRa.endPacket();
    }
  }
}

String readTHCSensor() {
  uint8_t result;
  float temperature, moisture, conductivity;

  result = node.readInputRegisters(0x0000, 3);
  if (result == node.ku8MBSuccess) {
    temperature = node.getResponseBuffer(0x00) / 10.0;
    moisture = node.getResponseBuffer(0x01) / 10.0;
    conductivity = node.getResponseBuffer(0x02);

    String sensorType = "THC-S";
    return clientName + ",Sensor:" + sensorType + ",Temp:" + String(temperature) + ",Moisture:" + String(moisture) + ",Conductivity:" + String(conductivity);
  } else {
    Serial.println("Failed to read from THC-S sensor");
    return "";
  }
}

String readBMESensor() {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  String sensorType = "BME280";
  return clientName + ",Sensor:" + sensorType + ",Temp:" + String(temperature) + ",Humidity:" + String(humidity) + ",Pressure:" + String(pressure);
}

# ESP32 LoRa MQTT Gateway Project

This project consists of two sketches for ESP32 devices that facilitate communication between LoRa and MQTT networks.

## Sketches

### 1. esp32_lora_client.ino

This sketch is designed for an ESP32 device that acts as a LoRa client. It performs the following functions:

- **LoRa Communication**: Initializes a LoRa module to communicate over a specified frequency band (915 MHz). It listens for incoming LoRa messages and sends responses back.
- **RS485 Communication**: Sets up RS485 communication using the Modbus protocol to read data from a THC-S sensor.
- **BME280 Sensor**: Initializes a BME280 sensor to read environmental data such as temperature, humidity, and pressure.
- **Data Transmission**: Every 30 seconds, it reads data from the THC-S sensor and the BME280 sensor, formats it into a string, and sends it over LoRa.

### 2. esp32_lora_mqtt_gateway.ino

This sketch is designed for an ESP32 device that acts as a LoRa to MQTT gateway. It performs the following functions:

- **WiFi and MQTT Setup**: Connects to a WiFi network and an MQTT broker. It subscribes to a specific MQTT topic to receive commands.
- **LoRa Communication**: Initializes a LoRa module to send and receive messages. Commands received via MQTT are sent over LoRa.
- **Data Processing**: Listens for incoming LoRa messages, parses the data to identify the sensor type (THC-S or BME280), and processes the data accordingly.
- **MQTT Publishing**: Sensor data is published to individual MQTT topics based on the sensor type and client name.

## Wiring Diagram for ESP32 Client

```
ESP32          LoRa Module
--------------------------
GPIO 5   <--> SCK
GPIO 19  <--> MISO
GPIO 27  <--> MOSI
GPIO 18  <--> SS
GPIO 14  <--> RST
GPIO 26  <--> DI0

ESP32          RS485
--------------------------
GPIO 16  <--> RX
GPIO 17  <--> TX

ESP32          BME280
--------------------------
I2C SDA <--> SDA
I2C SCL <--> SCL
```

## Wiring Diagram for ESP32 Gateway

```
ESP32          LoRa Module
--------------------------
GPIO 5   <--> SCK
GPIO 19  <--> MISO
GPIO 27  <--> MOSI
GPIO 18  <--> SS
GPIO 14  <--> RST
GPIO 26  <--> DI0
```

## Usage

- Ensure that the necessary libraries are installed for both sketches.
- Update the WiFi and MQTT credentials in `esp32_lora_mqtt_gateway.ino`.
- Upload the sketches to their respective ESP32 devices.
- Monitor the serial output for debugging and data verification.

## Docker Compose Setup

To build and run the Docker containers for the web application and Mosquitto MQTT server, follow these steps:

1. **Build and Start the Services**:

    ```bash
    docker-compose up -d
    ```

    This command builds the Docker image for the web application using the `Dockerfile` in the `mqtt-data-viewer` folder and starts both the `mosquitto` MQTT server and the `mqtt-data-viewer` web application containers in detached mode.

2. **Access the Application**:

    Open your web browser and navigate to `http://localhost:7180` to view the MQTT Data Viewer application.

3. **Stopping the Services**:

    To stop the running containers, use:

    ```bash
    docker-compose down
    ```

4. **Rebuilding the Docker Images**:

    If you make changes to the Dockerfile or the application, rebuild the images:

    ```bash
    docker-compose build
    ```

    Then, restart the services:

    ```bash
    docker-compose up -d
    ```

## Additional Notes

- **Environment Variables**: If your application requires environment variables (e.g., MQTT broker credentials), consider using Docker's `-e` flag or an environment file to pass them securely.

    ```bash
    docker-compose run -d \
      -e MQTT_USER=your_MQTT_USER \
      -e MQTT_PASSWORD=your_MQTT_PASSWORD \
      mqtt-data-viewer
    ```

- **Volume Mounting**: For development purposes, you might want to mount your local project directory into the Docker container to enable live reloading.

    ```bash
    docker-compose run -d \
      -v $(pwd)/mqtt-data-viewer:/app \
      mqtt-data-viewer
    ```

    This setup allows you to make changes to your code locally and have them reflected inside the container immediately.

- **Logging**: To view the logs from your Docker containers, use:

    ```bash
    docker-compose logs -f
    ```

    This command streams the logs, allowing you to monitor the application's behavior in real-time.

By following these steps, you can effectively containerize and deploy your MQTT Data Viewer web application using Docker Compose, ensuring both the application and the Mosquitto MQTT server run seamlessly together.

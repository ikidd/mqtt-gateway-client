const mqtt = require('mqtt');

const client = mqtt.connect('ws://mosquitto-mqtt-gw:9001/mqtt');

client.on('connect', () => {
  console.log('Connected to MQTT broker');

  setInterval(() => {
    const thcData = (Math.random() * 100).toFixed(2);
    const bmeData = (Math.random() * 100).toFixed(2);

    client.publish('esp32/test/THC-S/data', thcData);
    console.log(`Published THC-S data: ${thcData}`);

    client.publish('esp32/test/BME280/data', bmeData);
    console.log(`Published BME280 data: ${bmeData}`);
  }, 5000);
});

client.on('error', (err) => {
  console.error('Connection error:', err);
  client.end();
});

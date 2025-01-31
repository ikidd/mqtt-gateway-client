const mqtt = require('mqtt');

const client = mqtt.connect('ws://localhost:9001/mqtt');

client.on('connect', () => {
  console.log('Connected to MQTT broker');

  setInterval(() => {
    const temperature = (Math.random() * 40).toFixed(1);
    const moisture = (Math.random() * 100).toFixed(1);
    const conductivity = (Math.random() * 2000).toFixed(0);
    const humidity = (Math.random() * 100).toFixed(1);
    const pressure = (Math.random() * 1100).toFixed(1);

    client.publish('esp32/TestScript/THC-S/temperature', temperature);
    console.log(`Published THC-S temperature: ${temperature}`);

    client.publish('esp32/TestScript/THC-S/moisture', moisture);
    console.log(`Published THC-S moisture: ${moisture}`);

    client.publish('esp32/TestScript/THC-S/conductivity', conductivity);
    console.log(`Published THC-S conductivity: ${conductivity}`);

    client.publish('esp32/TestScript/BME280/temperature', temperature);
    console.log(`Published BME280 temperature: ${temperature}`);

    client.publish('esp32/TestScript/BME280/humidity', humidity);
    console.log(`Published BME280 humidity: ${humidity}`);

    client.publish('esp32/TestScript/BME280/pressure', pressure);
    console.log(`Published BME280 pressure: ${pressure}`);
  }, 5000);
});

client.on('error', (err) => {
  console.error('Connection error:', err);
  client.end();
});

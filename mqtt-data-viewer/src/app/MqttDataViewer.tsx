"use client";
import React, { useEffect, useState } from 'react';
import mqtt from 'mqtt';

const MqttDataViewer: React.FC = () => {
  const [thcData, setThcData] = useState<string[]>([]);
  const [bmeData, setBmeData] = useState<string[]>([]);

  useEffect(() => {
    const client = mqtt.connect('ws://localhost:9001/mqtt');

    client.on('connect', () => {
      console.log('Connected to MQTT broker');
      client.subscribe('esp32/+/THC-S/#');
      client.subscribe('esp32/+/BME280/#');
    });

    client.on('message', (topic, message) => {
      const data = message.toString();
      if (topic.includes('THC-S')) {
        setThcData((prevData) => [...prevData, data]);
      } else if (topic.includes('BME280')) {
        setBmeData((prevData) => [...prevData, data]);
      }c
    });

    return () => {
      client.end();
    };
  }, []);

  return (
    <div>
      <h1>MQTT Data Viewer</h1>
      <h2>THC-S Sensor Data</h2>
      <ul>
        {thcData.map((data, index) => (
          <li key={index}>{data}</li>
        ))}
      </ul>
      <h2>BME280 Sensor Data</h2>
      <ul>
        {bmeData.map((data, index) => (
          <li key={index}>{data}</li>
        ))}
      </ul>
    </div>
  );
};

export default MqttDataViewer;

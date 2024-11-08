# ESP32 Environmental Monitoring System

A real-time environmental monitoring system using ESP32, DHT11 sensor, MQTT protocol, and Node-RED dashboard for temperature and humidity tracking.
./video demonstration of my system.mp4
## Overview

- Built with ESP32 microcontroller and DHT11 sensor
- Uses MQTT for data transmission
- Node-RED dashboard for visualization
- MongoDB for data storage
- LED indicators for temperature alerts

## Requirements

### Hardware
- ESP32 Development Board
- DHT11 Sensor
- Connecting wires

### Software
- Arduino IDE
- Mosquitto MQTT Broker
- Node-RED
- MongoDB

## Quick Start

1. Connect DHT11 to ESP32:
   - VCC → 3.3V
   - GND → GND
   - DATA → GPIO14

2. Install required Arduino libraries:
   - DHT sensor library
   - PubSubClient
   - WiFi library

3. Configure MQTT settings in code
4. Upload code to ESP32
5. Start Node-RED dashboard
6. Access dashboard at `http://localhost:1880/ui`

## Features

- Real-time temperature & humidity monitoring
- Web dashboard visualization
- Historical data logging
- Automated LED alerts

## Author

Deepika Duggirala  
MSc Robotics  
Heriot Watt University  
dd3001@hw.ac.uk


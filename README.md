# Indoor quality monitoring using ESP32 + BME688
This project is an addendum of my cover letter for the job "Ingénieur IoT & IA embarquée (Smart Campus & plateforme IA durable)" from ULB, Brussels. 
## Pipeline overview (Homeserver + Wireguard VPN + MQTT + InfluxDB)
<img width="1103" height="513" alt="architecture drawio" src="https://github.com/user-attachments/assets/2cd821ad-8bab-422c-b3d8-72868dd36c3f" />
![2025-08-23-17-40-04-789](https://github.com/user-attachments/assets/2a1d8055-6fd8-4855-b124-71c8ab0ee2ee)


## Requirements
- ESP32 embedded device
- BME688 environmental sensor (example : https://www.gotronic.fr/art-capteur-ai-env--bme688-ferm--47926.htm)
- Functional ESP-IDF setup : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html

## Setup : 
- Clone the repo
```` 
$ mkdir -p ~/esp/projects
$ cd ~/esp/projects
$ git clone https://github.com/charliergi/esp32_bsec2_iaq.git
````
- Clone BSEC2 and BME88 repositories in subfolder components
````
$ cd esp32_bsec2_iaq
$ mkdir components && cd components
$ git clone --depth 1 --branch v1.10.2610 https://github.com/boschsensortec/Bosch-BSEC2-Library.git
$ git clone --depth 1 --branch v4.4.8 https://github.com/boschsensortec/BME68x_SensorAPI.git
````
- Create an .env file in in the main folder with the following variables 
````
WIFI_SSID={WIFI_SSID}
WIFI_PASS={WIFI_PASSWORD}
MQTT_USERNAME={MQTT_USERNAME}
MQTT_PASSWORD={MQTT_PASSWORD}
MQTT_BROKER=mqtt://{MQTT URL}:1883
````
- Build & flash the code
- Deploy a influxDB bucket named "esp32"
- Gather data with influxDB.
- Setup a dashboard via Grafana with custom alerts to monitor Indoor air quality:
- Examples :
<img width="1539" height="829" alt="image" src="https://github.com/user-attachments/assets/27f2e03b-0978-40fa-aef5-a97d9f9597ee" />
<img width="1524" height="421" alt="image" src="https://github.com/user-attachments/assets/ea4cf872-d99f-4c33-94a2-81fe3d2913ea" />


# Indoor quality monitoring using ESP32 + BME688
This project is an addendum of my cover letter for the job "Ingénieur IoT & IA embarquée (Smart Campus & plateforme IA durable)" from ULB, Brussels. 

## Pipeline overview (Homeserver + Wireguard VPN + MQTT + InfluxDB)
The C++ code uses Bosch’s BME688 and BSEC2 libraries and is programmed with the ESP-IDF framework. First, the BME688 is connected via SPI and collects data every 5 minutes. Each message is sent via MQTT to a MQTT broker through a secured WireGuard VPN connection and a public address.

After that, InfluxDB retrieves and stores measurements on my homeserver, which are then displayed through Grafana.
All these softwares run on my homeserver, using Docker.
Custom alerts can be defined on Grafana to automatically open windows or start the ventilation system.

<img width="1103" height="513" alt="architecture drawio" src="https://github.com/user-attachments/assets/2cd821ad-8bab-422c-b3d8-72868dd36c3f" />
<img width="296" height="605" alt="image" src="https://github.com/user-attachments/assets/87842cf5-e8a4-44f6-9b79-dc24d118d0b3" />


## Requirements
- ESP32 embedded device
- BME688 environmental sensor (example : https://www.gotronic.fr/art-capteur-ai-env--bme688-ferm--47926.htm)
- Functional ESP-IDF setup : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html

## Setup : 
- Clone the repo
```` 
$ mkdir -p ~/esp/projects
$ cd ~/esp/projects
$ git clone --recurse-submodules https://github.com/charliergi/esp32_bsec2_iaq.git
````
- Run the install script to copy CMakeLists.txt into the components folders.
````
$ cd esp32_bsec2_iaq
$ ./install.sh
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


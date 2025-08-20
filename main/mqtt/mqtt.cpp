#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "bme68x.h" // This is the header for the Bosch library you added
#include "rom/ets_sys.h" // For ets_delay_us

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "esp_mac.h"

static const char *TAG = "MQTT";


// --- Wi-Fi Definitions ---
char WIFI_SSID[32];
char WIFI_PASS[64];

// --- MQTT Definitions ---
char MQTT_BROKER[128];
char MQTT_USERNAME[32];
char MQTT_PASSWORD[32];

// Global MQTT client handle, made extern so it can be accessed from main.cpp
extern esp_mqtt_client_handle_t mqtt_client;
esp_mqtt_client_handle_t mqtt_client = NULL;



// --- Function to load variables from .env ---
void load_env_vars(void) {
    extern const uint8_t env_start[] asm("_binary__env_start");
    extern const uint8_t env_end[]   asm("_binary__env_end");
    const char* env_file = (const char*)env_start;
    size_t env_size = env_end - env_start;

    char line[128];
    const char *p = env_file;
    while (p < env_file + env_size) {
        const char *end = strchr(p, '\n');
        if (!end) {
            end = env_file + env_size;
        }
        size_t len = end - p;
        if (len > sizeof(line) - 1) {
            len = sizeof(line) - 1;
        }
        memcpy(line, p, len);
        line[len] = '\0';
        p = end + 1;

        char key[64], value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) == 2) {
            if (strcmp(key, "WIFI_SSID") == 0) {
                strncpy(WIFI_SSID, value, sizeof(WIFI_SSID) - 1);
                WIFI_SSID[sizeof(WIFI_SSID) - 1] = '\0';
            } else if (strcmp(key, "WIFI_PASS") == 0) {
                strncpy(WIFI_PASS, value, sizeof(WIFI_PASS) - 1);
                WIFI_PASS[sizeof(WIFI_PASS) - 1] = '\0';
            } else if (strcmp(key, "MQTT_USERNAME") == 0) {
                strncpy(MQTT_USERNAME, value, sizeof(MQTT_USERNAME) - 1);
                MQTT_USERNAME[sizeof(MQTT_USERNAME) - 1] = '\0';
            } else if (strcmp(key, "MQTT_PASSWORD") == 0) {
                strncpy(MQTT_PASSWORD, value, sizeof(MQTT_PASSWORD) - 1);
                MQTT_PASSWORD[sizeof(MQTT_PASSWORD) - 1] = '\0';
            } else if (strcmp(key, "MQTT_BROKER") == 0) {
                strncpy(MQTT_BROKER, value, sizeof(MQTT_BROKER) - 1);
                MQTT_BROKER[sizeof(MQTT_BROKER) - 1] = '\0';
            }
        }
    }
}

// --- Wi-Fi Event Handler ---
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Retrying to connect to the AP");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// --- Wi-Fi Initialization ---
void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            // .ssid and .password are set below
        },
    };
    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, WIFI_PASS, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

// --- MQTT Event Handler ---
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

// --- MQTT Initialization ---
void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = MQTT_BROKER;
    mqtt_cfg.credentials.username = MQTT_USERNAME;
    mqtt_cfg.credentials.authentication.password = (const char *)MQTT_PASSWORD;
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}
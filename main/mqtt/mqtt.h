#include "esp_event.h"
#include "mqtt_client.h"

// Declare the MQTT client as extern so it can be accessed from other files
extern esp_mqtt_client_handle_t mqtt_client;

void load_env_vars(void);

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

void wifi_init_sta(void);

void mqtt_app_start(void);

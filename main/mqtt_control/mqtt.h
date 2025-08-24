#include "esp_event.h"
#include "mqtt_client.h"
#include "esp_log.h"


// Declare the MQTT client as extern so it can be accessed from other files
extern esp_mqtt_client_handle_t mqtt_client;

void load_env_vars(void);

void wifi_init_sta(void);

void mqtt_app_start(void);

idf_component_register(SRCS "main.cpp"
                        "mqtt_control/mqtt.cpp"
                       PRIV_REQUIRES spi_flash driver Bosch-BME68x-Library Bosch-BSEC2-Library nvs_flash esp_netif esp_event esp_wifi mqtt espressif__arduino-esp32
                       INCLUDE_DIRS "." "mqtt_control"
                       EMBED_TXTFILES ".env")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -u _printf_float")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARDUINO=200")
target_link_libraries(${COMPONENT_LIB} PRIVATE m)

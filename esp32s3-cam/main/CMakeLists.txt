idf_component_register(SRCS camera.c server_stream.c main.c
                        PRIV_INCLUDE_DIRS .
                        PRIV_REQUIRES nvs_flash esp_psram esp_http_server esp_timer esp_wifi) 

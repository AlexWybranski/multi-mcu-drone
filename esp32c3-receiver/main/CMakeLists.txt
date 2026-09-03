set(srcs "main.cpp" "receiver.cpp")

set(requires "esp_wifi" "esp_netif" "nvs_flash" "esp_event")

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES "${requires}"
                    INCLUDE_DIRS ".")

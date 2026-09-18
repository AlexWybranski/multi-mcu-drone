set(srcs "main.cpp" "remote.cpp" "bluepadplatform.c")

set(requires "bluepad32" "btstack" "esp_wifi" "esp_driver_gpio")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}")

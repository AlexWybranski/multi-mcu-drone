#include "receiver.hpp"

extern "C" {
    int app_main(void);

    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>

    #include "nvs_flash.h"
}

int app_main(void) {
    esp_log_level_set("*", ESP_LOG_NONE);

    constexpr uint32_t DELAY_TO_FREE_CPU = 150U;

    Receiver::initUart();

    Receiver::initPwm();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    Receiver::initEspNowProtocol();

    while(1) {
        //EMPTY due to all logic in onReceive callback
        vTaskDelay(pdMS_TO_TICKS(DELAY_TO_FREE_CPU));
    }

    return 0;
}

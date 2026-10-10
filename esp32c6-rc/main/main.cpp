// Example file - Public Domain
// Need help? http://bit.ly/bluepad32-help

#include <cstdio>


#include "dronePacket.hpp"
#include "remote.hpp"

extern "C" {
    #include <stdlib.h>
    
    #include <btstack_port_esp32.h>
    #include <btstack_run_loop.h>
    #include <btstack_stdio_esp32.h>
    #include <hci_dump.h>
    #include <hci_dump_embedded_stdout.h>
    #include <uni.h>

    #include "nvs_flash.h"

    #include "sdkconfig.h"

    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>

    int app_main(void);

    struct uni_platform* get_my_platform(void);
}

constexpr size_t ESP_NOW_TASK_STACK_SIZE = 2048U;
constexpr size_t INDICATOR_TASK_STACK_SIZE = 2048U;
constexpr uint32_t ESP_NOW_TASK_DELAY_MS = 10U;

StackType_t txEspNowTask[ESP_NOW_TASK_STACK_SIZE*4U];
StaticTask_t txEspNowTaskBuffer;
TaskHandle_t txEspNowTaskHandle;

StackType_t indicatorTask[INDICATOR_TASK_STACK_SIZE*4U];
StaticTask_t indicatorTaskBuffer;
TaskHandle_t indicatorTaskHandle;

EventGroupHandle_t indicatorEventGroupHandle;
StaticEventGroup_t indicatorEventGroup;

void txEspNowTaskFunc(void* pvParameters);
void indicatorTaskFunc(void* pvParameters);

// Sanity check
static_assert(
#if defined(CONFIG_BLUEPAD32_PLATFORM_CUSTOM)
    true,
#else
    false,
#endif
    "Must use BLUEPAD32_PLATFORM_CUSTOM in menuconfig!"
);

int app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    RemoteControl::initRemoteConnection();

    RemoteControl::initExtAntenna();

    indicatorEventGroupHandle = xEventGroupCreateStatic(&indicatorEventGroup);

    txEspNowTaskHandle = xTaskCreateStaticPinnedToCore
    (
        txEspNowTaskFunc,
        "tx_esp_now_task",
        ESP_NOW_TASK_STACK_SIZE,
        nullptr,
        5,
        txEspNowTask,
        &txEspNowTaskBuffer,
        tskNO_AFFINITY
    );

    // Configure BTstack for ESP32 VHCI Controller
    btstack_init();

    // Must be called before uni_init()
    uni_platform_set_custom(get_my_platform());

    // Init Bluepad32.
    uni_init(0 /* argc */, NULL /* argv */);

    // Does not return.
    btstack_run_loop_execute();

    return 0;
}

void txEspNowTaskFunc(void* pvParameters) {
    using namespace ConstantValues;

    static_cast<void>(pvParameters);

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(ESP_NOW_TASK_DELAY_MS);

    EventBits_t indicatorBits;

    RemoteControl::initLed();

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        DroneControlPacket packet = RemoteControl::getAndClearPacket();

        indicatorBits = xEventGroupGetBits(indicatorEventGroupHandle);

        if(indicatorBits & xPadConnected) {
            RemoteControl::sendPacket(&packet);
            gpio_set_level(GREEN_LED, SET_HIGH);
            esp_rom_printf("[PACKET] | Throttle: %u, Pitch: %d, Roll: %d, Buttons: %u, CRC: %X\n", packet.controlData.data.throttle, packet.controlData.data.pitch, packet.controlData.data.roll, packet.controlData.data.buttonControlReg, packet.crcValue);
        } else {
            esp_rom_printf("Pad NOT connected\n");
            gpio_set_level(GREEN_LED, SET_LOW);
        }

        xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
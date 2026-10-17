#include "camera.h"
#include "server_stream.h"

void app_main(void)
{
    if(ESP_OK != init_camera()) {
        return;
    }

    initWifi();
    startCamServer();

    //192.168.4.1

    while (1)
    {
        vTaskDelay(60000 / portTICK_RATE_MS);
    }
}
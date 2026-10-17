#ifndef SERVER_H
#define SERVER_H

#include <nvs_flash.h>
#include "esp_http_server.h"
#include "esp_wifi.h" // IWYU pragma: keep
#include "esp_wifi_default.h" // IWYU pragma: keep
#include "esp_wifi_types_generic.h" // IWYU pragma: keep
#include "esp_camera.h" // IWYU pragma: keep
#include "esp_timer.h" // IWYU pragma: keep
#include <esp_log.h>

void initWifi(void);

httpd_handle_t startCamServer(void);

esp_err_t jpg_stream_httpd_handler(httpd_req_t *req);

#endif
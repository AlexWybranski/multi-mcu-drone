#include "server_stream.h"

#define PART_BOUNDARY "123456789000000000000987654321"

static const char *TAG = "example:take_picture";
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %zu\r\n\r\n";

void initWifi(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifiCfg = {};

    strcpy((char*)wifiCfg.ap.ssid, "ESP_HotSpot");
    strcpy((char*)wifiCfg.ap.password, "cameratest1234");
    wifiCfg.ap.ssid_len = strlen("ESP_HotSpot");
    wifiCfg.ap.channel = 10;
    wifiCfg.ap.max_connection = 2;
    wifiCfg.ap.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifiCfg));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(40));

    ESP_ERROR_CHECK(esp_wifi_set_channel(10, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N) );

    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
}

httpd_handle_t startCamServer(void) {
    httpd_config_t serverConfig = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t serverHandle = NULL;

    ESP_ERROR_CHECK(httpd_start(&serverHandle, &serverConfig));

    httpd_uri_t stream_uri = {
        "/",
        HTTP_GET,
        jpg_stream_httpd_handler,
        NULL
    };

    httpd_register_uri_handler(serverHandle, &stream_uri);

    return serverHandle;
}

esp_err_t jpg_stream_httpd_handler(httpd_req_t *req){
    camera_fb_t* fb = NULL;
    esp_err_t res = ESP_OK;
    //size_t jpg_buf_len = 0;
    //uint8_t* jpg_buf = NULL;
    char part_buf[64];
    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    while(true){
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        //jpg_buf_len = fb->len;
        //jpg_buf = fb->buf;

        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));


        int hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, fb->len);
        if(hlen < 0 || hlen >= sizeof(part_buf)){
            ESP_LOGE(TAG, "Header truncated (%d bytes needed >= %zu buffer)",
                     hlen, sizeof(part_buf));
            res = ESP_FAIL;
        } else {
            res = httpd_resp_send_chunk(req, part_buf, (size_t)hlen);
        }

        res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);

        esp_camera_fb_return(fb);

        if(res != ESP_OK){
            break;
        }

        // FPS CALC ↓
        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        float fps = frame_time > 0 ? 1000.0f / (float)frame_time : 0.0f;
        ESP_LOGI(TAG, "MJPG: %uKB %ums (%.1ffps)",
            (uint32_t)(fb->len/1024),
            (uint32_t)frame_time, fps);
    }

    last_frame = 0;
    return res;
}
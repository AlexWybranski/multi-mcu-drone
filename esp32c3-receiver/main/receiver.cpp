#include "receiver.hpp"

#include <cstring>

extern "C" {
    #include "esp_wifi.h"
}

void Receiver::initEspNowProtocol() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );

    ESP_ERROR_CHECK(esp_now_init());

    ESP_ERROR_CHECK(esp_now_register_recv_cb(Receiver::onReceive));
}

void Receiver::onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int data_len) {
    DroneControlPacket receivedPacket;

    if(data_len == sizeof(DroneControlPacket)) {
        std::memcpy(&receivedPacket, data, data_len);
        
        Receiver::m_packet = receivedPacket;

        esp_rom_printf("[RCV] Throttle: %u | Pitch: %u | Roll: %u | Buttons: %u | CRC: %X\n", Receiver::m_packet.throttle, Receiver::m_packet.pitch, Receiver::m_packet.roll, Receiver::m_packet.buttonControlReg, Receiver::m_packet.crcValue);
    } else {
        //DROP PACKET
    }
}

void Receiver::uartStream() {

}
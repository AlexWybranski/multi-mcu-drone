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

        int wyslane = uart_write_bytes(m_uart_num, data, static_cast<size_t>(data_len));

        esp_rom_printf("UART wyslal bajtow: %d\n", wyslane);

        esp_rom_printf("[RCV] Throttle: %u | Pitch: %u | Roll: %u | Buttons: %u | CRC: %X\n", Receiver::m_packet.throttle, Receiver::m_packet.pitch, Receiver::m_packet.roll, Receiver::m_packet.buttonControlReg, Receiver::m_packet.crcValue);
    } else {
        //DROP PACKET
    }
}

void Receiver::initUart() {
    constexpr int uart_buffer_tx_size = 256;
    constexpr int uart_buffer_rx_size = 256;
    constexpr int UART_BAUD_RATE = 115200;
    
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_rx_size, uart_buffer_tx_size, 0, nullptr, 0));

    uart_config_t uart_config = {};
    uart_config.baud_rate = UART_BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;

    ESP_ERROR_CHECK(uart_param_config(m_uart_num, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}
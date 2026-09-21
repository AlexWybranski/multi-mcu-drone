#include "receiver.hpp"

#include <cstring>

extern "C" {
    #include "esp_wifi.h"
    #include "driver/ledc.h"
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
    using namespace PWM_SETUP;

    DroneControlPacket receivedPacket;

    if(data_len == sizeof(DroneControlPacket)) {
        uart_write_bytes(m_uart_num, data, static_cast<size_t>(data_len));
        
        std::memcpy(&receivedPacket, data, data_len);
        
        Receiver::m_packet = receivedPacket;

        if(m_packet.buttonControlReg & ControlRegister::CAM_UP) {
            duty-=PWM_STEP;
        }
        
        if(m_packet.buttonControlReg & ControlRegister::CAM_DOWN) {
            duty+=PWM_STEP;
        }

        if (duty < PWM_MIN_DUTY) {
            duty = PWM_MIN_DUTY;
        }

        if (duty > PWM_MAX_DUTY) {
            duty = PWM_MAX_DUTY; 
        }

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));

        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
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

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void Receiver::initPwm() {
    using namespace PWM_SETUP;

    ledc_timer_config_t timer_config{};

    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.duty_resolution = LEDC_TIMER_13_BIT;
    timer_config.timer_num = LEDC_TIMER_0;
    timer_config.freq_hz = PWM_HZ;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));
    

    ledc_channel_config_t ledc_channel{};

    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.gpio_num = PWM_OUT_PIN;
    ledc_channel.duty = PWM_MIN_DUTY;
    ledc_channel.hpoint = PWM_HPOINT;

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

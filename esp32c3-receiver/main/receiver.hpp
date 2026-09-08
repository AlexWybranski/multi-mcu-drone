#include "dronePacket.hpp"

extern "C" {
    #include "esp_now.h"
    #include "driver/uart.h"
}

namespace PWM_SETUP {
    constexpr uint32_t PWM_MIN_DUTY = 250U;
    constexpr uint32_t PWM_MAX_DUTY = 700U;
    constexpr uint32_t PWM_STEP = 10U;
    constexpr uint32_t PWM_OUT_PIN = 42U;
    constexpr uint32_t PWM_HZ = 50U;
    constexpr uint32_t PWM_HPOINT = 0U;
}

class Receiver {
    private:
        static inline DroneControlPacket m_packet;
        static inline const uart_port_t m_uart_num = UART_NUM_1;
        static inline uint32_t duty = PWM_SETUP::PWM_MIN_DUTY;
    public:
        static void initEspNowProtocol();

        static void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int data_len);

        static void initUart();

        static void initPwm();
};
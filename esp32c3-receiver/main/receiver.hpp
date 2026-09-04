#include "dronePacket.hpp"


extern "C" {
    #include "esp_now.h"
    #include "driver/uart.h"
}

class Receiver {
    private:
        static inline DroneControlPacket m_packet;
        static inline const uart_port_t m_uart_num = UART_NUM_1;
    public:
        static void initEspNowProtocol();

        static void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int data_len);

        static void initUart();
};
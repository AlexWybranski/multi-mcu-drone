#include "dronePacket.hpp"


extern "C" {
    #include "esp_now.h"
}

class Receiver {
    private:
        static inline DroneControlPacket m_packet;
    public:
        static void initEspNowProtocol();

        static void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int data_len);

        static void uartStream();
};
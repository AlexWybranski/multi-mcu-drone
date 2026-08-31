#ifndef PACKET_HPP
#define PACKET_HPP
#include <cstdint>

namespace Buttons {
    constexpr uint8_t LB = 0;
    constexpr uint8_t RB = 1U;
    constexpr uint8_t UP = 2U;
    constexpr uint8_t DOWN = 3U;
    constexpr uint8_t CROSS = 4U;
    constexpr uint8_t SQUARE = 5U;
    constexpr uint8_t TRIANGLE = 6U;
    constexpr uint8_t CIRCLE = 7U;
}

struct __attribute__((packed)) DroneControlPacket {
    uint8_t throttle;
    uint8_t roll;
    uint8_t pitch;
    uint8_t buttonControlReg;

    uint32_t crcValue;
};

#endif
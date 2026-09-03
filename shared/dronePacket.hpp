#ifndef PACKET_HPP
#define PACKET_HPP
#include <cstdint>

namespace Buttons {
    constexpr uint8_t YAW_LEFT = 0;
    constexpr uint8_t YAW_RIGHT = 1U;
    constexpr uint8_t CAM_UP = 2U;
    constexpr uint8_t CAM_DOWN = 3U;
    constexpr uint8_t START_STOP_ENGINE = 4U;
    constexpr uint8_t Res0 = 5U;
    constexpr uint8_t Res1 = 6U;
    constexpr uint8_t NO_PAD = 7U;
}

struct __attribute__((packed)) DroneControlPacket {
    uint8_t throttle{128};
    uint8_t roll{128};
    uint8_t pitch{128};
    uint8_t buttonControlReg{0};

    uint32_t crcValue{0xFFFFFFFF};
};

#endif

#ifndef PACKET_HPP
#define PACKET_HPP
#include <cstdint>

namespace ControlRegister {
    constexpr uint8_t YAW_LEFT = (0b1U << 0U);
    constexpr uint8_t YAW_RIGHT = (0b1U << 1U);
    constexpr uint8_t CAM_UP = (0b1U << 2U);
    constexpr uint8_t CAM_DOWN = (0b1U << 3U);
    constexpr uint8_t START_STOP_ENGINE = (0b1U << 4U);
    constexpr uint8_t Res0 = (0b1U << 5U);
    constexpr uint8_t Res1 = (0b1U << 6U);
    constexpr uint8_t NO_PAD = (0b1U << 7U);
}

struct __attribute__((packed)) DroneControlPacket {
    uint8_t throttle{128};
    uint8_t roll{128};
    uint8_t pitch{128};
    uint8_t buttonControlReg{0};

    uint32_t crcValue{0xFFFFFFFF};
};

#endif

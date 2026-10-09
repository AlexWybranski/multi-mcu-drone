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

namespace packetStructure {
    constexpr uint32_t PACKET_SIZE = 8U;
    constexpr uint32_t DATA_LENGTH = 4U;
}

struct __attribute__((packed)) DroneControlData {
    uint16_t throttle : 8;
    int16_t roll : 9;
    int16_t pitch : 9;
    uint8_t buttonControlReg : 6;
};

union ControlDataUnion {
    DroneControlData data;
    uint32_t dataRaw;
};

struct __attribute__((packed)) DroneControlPacket {
    ControlDataUnion controlData;
    uint32_t crcValue{0xFFFFFFFF};
};

#endif

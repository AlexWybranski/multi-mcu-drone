#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <cstdint>

namespace packetStructure {
    constexpr uint32_t PACKET_SIZE = 8U;
    constexpr uint32_t DATA_LENGTH = 4U;
}

struct __attribute__((packed)) ControlTelemetry {
    uint8_t throttle;
    uint8_t roll;
    uint8_t pitch;
    uint8_t yaw;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    bool yaw_left;
    bool yaw_right;
    bool start_stop_engine;
};

#endif
#ifndef MOTORS_HPP
#define MOTORS_HPP

#include <cstdint>

namespace MOTOR_CONSTANTS {
    constexpr uint16_t yaw = 32;
}

enum class Motor : uint32_t {
    RF = 1,
    LT = 2,
    LF = 3,
    RT = 4
};

#endif
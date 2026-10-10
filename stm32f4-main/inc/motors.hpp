#ifndef MOTORS_HPP
#define MOTORS_HPP

#include <cstdint>

#include "dronePacket.hpp"

namespace MOTOR_SETUP {
    static constexpr int16_t DUTY_MIN = 50U;
    static constexpr int16_t DUTY_MAX = 500U;
    static constexpr int16_t YAW_LEFT_STEP = 32U;
    static constexpr int16_t YAW_RIGHT_STEP = -32;
}

struct MotorPWMs {
    int16_t motorRigFro_1, motorLefRea_2, motorLefFro_3, motorRigRea_4;
};

class MotorMixer {
    private:
        static inline int16_t clamp(int16_t val) {
            using namespace MOTOR_SETUP;

            if (val > DUTY_MAX) {
                val = DUTY_MAX;
            }
            if (val < DUTY_MIN) {
                val = DUTY_MIN;
            }
            return val;
        }

    public:
        static MotorPWMs mixMotors(const DroneControlData& data);
};

#endif
#ifndef MOTORS_HPP
#define MOTORS_HPP

#include <cstdint>

#include "dronePacket.hpp"
#include "pid.hpp"
#include "tim.hpp"

namespace MOTOR_SETUP {
    static constexpr int16_t DUTY_MIN = 25U;
    static constexpr int16_t DUTY_MAX = TIM_SETUP::MAX_DUTY - 25U;
    static constexpr int16_t YAW_LEFT_STEP = 32U;
    static constexpr int16_t YAW_RIGHT_STEP = -32;
}

struct MotorInputs {
    float motorRigFro_1, motorLefRea_2, motorLefFro_3, motorRigRea_4;
};

struct MotorPWMs {
    uint16_t motorRigFro_1, motorLefRea_2, motorLefFro_3, motorRigRea_4;
};

class MotorMixer {
    private:
        static inline float clamp(float val) {
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
        static void mixMotors(const DroneControlData& data, const PidOutput& input, MotorPWMs& motorsOut);
};

#endif
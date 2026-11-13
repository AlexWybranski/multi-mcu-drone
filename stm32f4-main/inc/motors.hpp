#ifndef MOTORS_HPP
#define MOTORS_HPP

#include <cstdint>

#include "dronePacket.hpp"
#include "pid.hpp"
#include "tim.hpp" // IWYU pragma: keep

namespace MOTOR_SETUP {
    static constexpr int16_t DUTY_MIN = 25U;
    static constexpr int16_t DUTY_MAX = 475U;
    static constexpr float YAW_LEFT_STEP = 32.0F;
    static constexpr float YAW_RIGHT_STEP = -32.0F;
}

struct MotorInputs {
    float motorRigFro, motorLefRea, motorLefFro, motorRigRea;
};

/*
    Decription of physical connection to ESC
    1 - CW  - Right Rear
    2 - CCW - Right Front
    3 - CCW - Left Rear
    4 - CW  - Left Front
*/
struct MotorPWMs {
    uint16_t motor_1, motor_2, motor_3, motor_4;
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
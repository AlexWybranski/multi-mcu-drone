#include "motors.hpp"

void MotorMixer::mixMotors(const DroneControlData& data, const PidOutput& input, MotorPWMs& motorsOut) {
    using namespace MOTOR_SETUP;

    MotorInputs motors{};

    auto l_throttle = static_cast<int16_t>(data.throttle << 1U);
    int16_t l_yaw = 0;

    if (data.buttonControlReg & ControlRegister::YAW_LEFT) {
        l_yaw = YAW_LEFT_STEP;
    }
    if (data.buttonControlReg & ControlRegister::YAW_RIGHT) {
        l_yaw = YAW_RIGHT_STEP;
    }

    motors.motorRigFro_1 = static_cast<int16_t>(l_throttle - input.roll + input.pitch + l_yaw); //RF
    motors.motorLefRea_2 = static_cast<int16_t>(l_throttle + input.roll - input.pitch + l_yaw); //LR
    motors.motorLefFro_3 = static_cast<int16_t>(l_throttle + input.roll + input.pitch - l_yaw); //LF
    motors.motorRigRea_4 = static_cast<int16_t>(l_throttle - input.roll - input.pitch - l_yaw); //RR

    motors.motorRigFro_1 = clamp(motors.motorRigFro_1);
    motors.motorLefRea_2 = clamp(motors.motorLefRea_2);
    motors.motorLefFro_3 = clamp(motors.motorLefFro_3);
    motors.motorRigRea_4 = clamp(motors.motorRigRea_4);

    motorsOut.motorRigFro_1 = static_cast<uint16_t>(motors.motorRigFro_1);
    motorsOut.motorLefRea_2 = static_cast<uint16_t>(motors.motorLefRea_2);
    motorsOut.motorLefFro_3 = static_cast<uint16_t>(motors.motorLefFro_3);
    motorsOut.motorRigRea_4 = static_cast<uint16_t>(motors.motorRigRea_4);
}
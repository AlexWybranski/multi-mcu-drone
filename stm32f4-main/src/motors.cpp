#include "motors.hpp"

MotorPWMs MotorMixer::mixMotors(const DroneControlData& data) {
    using namespace MOTOR_SETUP;

    MotorPWMs motors;

    int16_t l_throttle = static_cast<int16_t>(data.throttle << 1);
    int16_t l_yaw;

    if (data.buttonControlReg & ControlRegister::YAW_LEFT) {
        l_yaw = YAW_LEFT_STEP;
    } else if (data.buttonControlReg & ControlRegister::YAW_RIGHT) {
        l_yaw = YAW_RIGHT_STEP;
    } else {
        l_yaw = 0;
    }

    motors.motorRigFro_1 = static_cast<int16_t>(l_throttle - data.roll + data.pitch + l_yaw); //RF
    motors.motorLefRea_2 = static_cast<int16_t>(l_throttle + data.roll - data.pitch + l_yaw); //LR
    motors.motorLefFro_3 = static_cast<int16_t>(l_throttle + data.roll + data.pitch - l_yaw); //LF
    motors.motorRigRea_4 = static_cast<int16_t>(l_throttle - data.roll - data.pitch - l_yaw); //RR

    motors.motorRigFro_1 = clamp(motors.motorRigFro_1);
    motors.motorLefRea_2 = clamp(motors.motorLefRea_2);
    motors.motorLefFro_3 = clamp(motors.motorLefFro_3);
    motors.motorRigRea_4 = clamp(motors.motorRigRea_4);

    return motors;
}
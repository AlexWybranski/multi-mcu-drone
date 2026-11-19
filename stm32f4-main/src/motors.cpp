#include "motors.hpp"

// DEBUG VARIABLES
float motor1, motor2, motor3, motor4; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void MotorMixer::mixMotors(const DroneControlData& data, const PidOutput& input, MotorPWMs& motorsOut) {
    using namespace MOTOR_SETUP;

    MotorInputs motors{};

    auto l_throttle = static_cast<float>(data.throttle << 1U);
    float l_yaw = 0;

    if (data.buttonControlReg & ControlRegister::YAW_LEFT) {
        l_yaw = YAW_LEFT_STEP;
    }
    if (data.buttonControlReg & ControlRegister::YAW_RIGHT) {
        l_yaw = YAW_RIGHT_STEP;
    }

    motors.motorRigFro = static_cast<float>(l_throttle - input.roll + input.pitch + l_yaw); //RF
    motors.motorLefRea = static_cast<float>(l_throttle + input.roll - input.pitch + l_yaw); //LR
    motors.motorLefFro = static_cast<float>(l_throttle + input.roll + input.pitch - l_yaw); //LF
    motors.motorRigRea = static_cast<float>(l_throttle - input.roll - input.pitch - l_yaw); //RR

    motors.motorRigFro = clamp(motors.motorRigFro);
    motors.motorLefRea = clamp(motors.motorLefRea);
    motors.motorLefFro = clamp(motors.motorLefFro);
    motors.motorRigRea = clamp(motors.motorRigRea);

    motor1 = motors.motorRigRea;
    motor2 = motors.motorRigFro;
    motor3 = motors.motorLefRea;
    motor4 = motors.motorLefFro;

    motorsOut.motor_1 = static_cast<uint16_t>(motors.motorRigRea);
    motorsOut.motor_2 = static_cast<uint16_t>(motors.motorRigFro);
    motorsOut.motor_3 = static_cast<uint16_t>(motors.motorLefRea);
    motorsOut.motor_4 = static_cast<uint16_t>(motors.motorLefFro);
}
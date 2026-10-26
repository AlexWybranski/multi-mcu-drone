#include "pid.hpp"

// DEBUG VARIABLES
// float real_angle_x, real_angle_y;
// float set_angle_x, set_angle_y;

void Pid::outputClamp(float& candidate, float& integral, float currentIntegral) {
    using namespace PID_SETUP;

    if (candidate > MAX_OUTPUT) {
        candidate = MAX_OUTPUT;
        integral -= currentIntegral;
    }
    if (candidate < MIN_OUTPUT) {
        candidate = MIN_OUTPUT;
        integral -= currentIntegral;
    }
}

void Pid::calculateAxisValuesAcc(ImuDataNormalized& imuData) {
    acc.x = atan2f(imuData.acc_y, sqrtf((imuData.acc_z * imuData.acc_z) + (imuData.acc_x * imuData.acc_x))) * RadianToDegree;
    acc.y = atan2f((imuData.acc_x * -1), sqrtf((imuData.acc_z * imuData.acc_z) + (imuData.acc_y * imuData.acc_y))) * RadianToDegree;
}

void Pid::calculateAxisValuesGyro(ImuDataNormalized& imuData) {
    gyro.x = imuData.gyro_x * dt;
    gyro.y = imuData.gyro_y * dt;
}

void Pid::complementaryFilter() {
    realAngle.y = alpha * (realAngle.y + gyro.y) + (1 - alpha) * acc.y;
    realAngle.x = alpha * (realAngle.x + gyro.x) + (1 - alpha) * acc.x;

    // DEBUG VARIABLES
    // real_angle_x = realAngle.x;
    // real_angle_y = realAngle.y;
}

void Pid::inputToAngle(int16_t roll, int16_t pitch) {
    setAngle.x = roll * PID_SETUP::SCALE;
    setAngle.y = pitch * PID_SETUP::SCALE;

    // DEBUG VARIABLES
    // set_angle_x = setAngle.x;
    // set_angle_y = setAngle.y;
}

void Pid::calculatePID() {
    float outP_x{0}, outI_x{0}, outD_x{0};
    float outP_y{0}, outI_y{0}, outD_y{0};

    // ERRORS
    float errorY = setAngle.y - realAngle.y;
    float errorX = setAngle.x - realAngle.x;

    // P
    outP_x = Kp * errorX;
    outP_y = Kp * errorY;

    // I
    float currentIntegralX = (errorX * dt);
    float currentIntegralY = (errorY * dt);
    integralX += currentIntegralX;
    integralY += currentIntegralY;

    outI_x = Ki * integralX;
    outI_y = Ki * integralY;

    // D
    outD_x = Kd * (errorX - prevErrorX) / dt;
    outD_y = Kd * (errorY - prevErrorY) / dt;

    // ACCUMULATE
    prevErrorX = errorX;
    prevErrorY = errorY;

    float outRoll = outP_x + outI_x + outD_x;
    float outPitch = outP_y + outI_y + outD_y;

    // CLAMP AND ANTI-WINDUP
    outputClamp(outRoll, integralX, currentIntegralX);
    outputClamp(outPitch, integralY, currentIntegralY);

    // OUTPUT
    pidOutput.roll = outRoll;
    pidOutput.pitch = outPitch;
}

void Pid::resetIntegrals() {
    integralX = 0.0f;
    integralY = 0.0f;
}
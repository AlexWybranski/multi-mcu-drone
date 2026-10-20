#include "pid.hpp"

float real_angle_x, real_angle_y;

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

    real_angle_x = realAngle.x;
    real_angle_y = realAngle.y;
}
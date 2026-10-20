#ifndef PID_HPP
#define PID_HPP

#include "imu.hpp" // IWYU pragma: keep
#include <bit> // IWYU pragma: keep
#include <cmath>

struct AxisValues {
    float x;
    float y;
};

class Pid {
    private:
        static constexpr float dt = 0.001f;
        static constexpr float RadianToDegree = 180.0f/3.14159265f;
        AxisValues gyro{0,0};
        AxisValues acc{0,0};
        AxisValues realAngle{0,0};
        float alpha = 0.995f;

    public:
        Pid() = default;
        ~Pid() = default;
        Pid(const Pid& other) = delete;
        Pid& operator=(const Pid& other) = delete;
        Pid(Pid&& other) = delete;
        Pid& operator=(Pid&& other) = delete;

        void calculateAxisValuesAcc(ImuDataNormalized& imuData);

        void calculateAxisValuesGyro(ImuDataNormalized& imuData);

        void complementaryFilter();
};

#endif
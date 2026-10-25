#ifndef PID_HPP
#define PID_HPP

#include "imu.hpp" // IWYU pragma: keep

#include <cmath>

namespace PID_SETUP {
    constexpr int16_t MAX_INPUT = 255U;
    constexpr int16_t MAX_ANGLE = 45U;
    constexpr float SCALE = static_cast<float>(MAX_ANGLE) / MAX_INPUT;
    constexpr float MAX_OUTPUT = 250.0f;
    constexpr float MIN_OUTPUT = -250.0f;
};

struct AxisValues {
    float x;
    float y;
};

struct PidOutput {
    float roll;
    float pitch;
};

class Pid {
    private:
        static constexpr float dt = 0.001f;
        static constexpr float RadianToDegree = 180.0f/3.14159265f;
        static constexpr float Kp = 1.0f;
        static constexpr float Ki = 0.0f;
        static constexpr float Kd = 0.0f;

        AxisValues gyro{0.0f,0.0f};
        AxisValues acc{0.0f,0.0f};
        AxisValues realAngle{0.0f,0.0f};
        AxisValues setAngle{0.0f,0.0f};

        float alpha = 0.995f;
        float prevErrorX{0.0f}, prevErrorY{0.0f};
        float integralX{0.0f}, integralY{0.0f};

        void outputClamp(float& candidate, float& integral, float currentIntegral);

    public:
        Pid() = default;
        ~Pid() = default;
        Pid(const Pid& other) = delete;
        Pid& operator=(const Pid& other) = delete;
        Pid(Pid&& other) = delete;
        Pid& operator=(Pid&& other) = delete;

        PidOutput pidOutput;

        void calculateAxisValuesAcc(ImuDataNormalized& imuData);

        void calculateAxisValuesGyro(ImuDataNormalized& imuData);

        void complementaryFilter();

        void inputToAngle(int16_t roll, int16_t pitch);

        void calculatePID();

        void resetIntegrals();
};

#endif
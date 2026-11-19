#ifndef PID_HPP
#define PID_HPP

#include "imu.hpp" // IWYU pragma: keep

#include <cmath>

namespace PID_SETUP {
    constexpr float MAX_INPUT = 255.0F;
    constexpr float MAX_ANGLE = 45.0F;
    constexpr float SCALE = MAX_ANGLE / MAX_INPUT;
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
        static constexpr float dt = 0.002F;
        static constexpr float PI = 3.14159265F;
        static constexpr float RadianToDegree = 180.0F/PI;
        static constexpr float Kp = 1.5F;
        static constexpr float Ki = 0.1F;
        static constexpr float Kd = 0.2F;

        static constexpr float CUTOFF_FREQ = 50.0F;
        
        AxisValues gyro{0.0F,0.0F};
        AxisValues acc{0.0F,0.0F};
        AxisValues realAngle{0.0F,0.0F};
        AxisValues setAngle{0.0F,0.0F};
        
        constexpr float calculateAlphaD() {
            return ((2 * PI) * CUTOFF_FREQ * dt) / (((2 * PI) * CUTOFF_FREQ * dt) + 1);   
        }

        void outputClamp(float& candidate, float& integral, float currentIntegral);
        float filterD(float& currentD, float& lastD);

        float alpha = 0.995F; // for complementary filter (imu and acc)
        float alphaD = calculateAlphaD(); // for part D of PID algorithm (D filtering)
        float prevErrorX{0.0F}, prevErrorY{0.0F};
        float integralX{0.0F}, integralY{0.0F};
        float lastD_x{0.0F}, lastD_y{0.0F}; // last part D calculation

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
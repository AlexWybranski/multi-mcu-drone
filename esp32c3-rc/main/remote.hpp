#ifndef REMOTE_HPP
#define REMOTE_HPP
#include <cstdint>

#include "dronePacket.hpp"

namespace ConstantValues {
    constexpr uint32_t DEAD_ZONE = 50;
};

namespace ButtonMasks {
    //BUTTONS check masks
    constexpr uint32_t BUTTON_A = (0b1U << 0);
    constexpr uint32_t BUTTON_B = (0b1U << 1U);
    constexpr uint32_t BUTTON_X = (0b1U << 2U);
    constexpr uint32_t BUTTON_Y = (0b1U << 3U);
    constexpr uint32_t BUTTON_SHOULDER_L = (0b1U << 4U);
    constexpr uint32_t BUTTON_SHOULDER_R = (0b1U << 5U);

    //DPAD check masks
    constexpr uint32_t DPAD_UP = (0b1U << 0);
    constexpr uint32_t DPAD_DOWN = (0b1U << 1U);
};

class RemoteControl {
    private:
    
    static inline uint8_t scaleAxisValue(int32_t axisValue) {
        return static_cast<uint8_t>((axisValue >> 2U) + 128U);
    }
    
    static inline void calculateCRC(DroneControlPacket* dronePacket) {
        
    }
    
    static inline void sendPacket(DroneControlPacket dronePacket) {
        
    }

    static inline int32_t applyDeadzone(int32_t axis) {
        using namespace ConstantValues;
        if (axis < DEAD_ZONE && axis > -DEAD_ZONE) {
            return 0; 
        } else {
            return axis;
        }   
    }
    
    public:
        RemoteControl() = default;
        ~RemoteControl() = default;
        
        static inline int32_t DEAD_ZONE = 32;

        static void handlePadData(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons);
};

#endif
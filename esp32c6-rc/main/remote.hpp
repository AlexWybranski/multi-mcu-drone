#ifndef REMOTE_HPP
#define REMOTE_HPP
#include <cstdint>
#include <mutex>
#include <cstring>
#include <array>

#include "dronePacket.hpp"

extern "C" {
    #include "esp_now.h"
    #include "esp_rom_crc.h"
    #include "driver/gpio.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/event_groups.h"
}

extern EventGroupHandle_t indicatorEventGroupHandle;

namespace ConstantValues {
    constexpr int32_t DEAD_ZONE = 50;
    constexpr int32_t NEUTRAL_AXIS_VAL = 128U;

    constexpr size_t MAC_LENGTH = 6U; 
    constexpr std::array<uint8_t, MAC_LENGTH> RECEIVER_MAC = {0x10,0xBD,0xA3,0x90,0xE2,0xB8};

    constexpr uint32_t PACKET_DATA_SIZE = 4U;
    constexpr size_t PACKET_SIZE = sizeof(DroneControlPacket);
    
    constexpr uint8_t N_THROTTLE = 128U;
    constexpr uint8_t N_ROLL = 128U;
    constexpr uint8_t N_PITCH = 128U;
    constexpr uint8_t N_BUTTON_REG = 0U;

    constexpr std::array<uint8_t, PACKET_DATA_SIZE> NEUTRAL_DATA{N_THROTTLE, N_ROLL, N_PITCH, N_BUTTON_REG};

    constexpr uint32_t N_CRC_CALC_VALUE = 0x0U;

    constexpr uint8_t RST_BUTTONS = ~(ControlRegister::CAM_DOWN | ControlRegister::CAM_UP | ControlRegister::YAW_LEFT | ControlRegister::YAW_RIGHT | ControlRegister::START_STOP_ENGINE);

    constexpr gpio_num_t GREEN_LED = GPIO_NUM_21;
    constexpr uint32_t SET_HIGH = 1;
    constexpr uint32_t SET_LOW = 0;

    constexpr EventBits_t xPadConnected = (0b1U << 0U); 
};

namespace ButtonMasks {
    //BUTTONS check masks
    constexpr uint32_t BUTTON_A = (0b1U << 0U);
    constexpr uint32_t BUTTON_B = (0b1U << 1U);
    constexpr uint32_t BUTTON_X = (0b1U << 2U);
    constexpr uint32_t BUTTON_Y = (0b1U << 3U);
    constexpr uint32_t BUTTON_SHOULDER_L = (0b1U << 4U);
    constexpr uint32_t BUTTON_SHOULDER_R = (0b1U << 5U);

    //DPAD check masks
    constexpr uint32_t DPAD_UP = (0b1U << 0U);
    constexpr uint32_t DPAD_DOWN = (0b1U << 1U);
};

class RemoteControl {
    private:
        static inline int16_t scaleAxisValue(int32_t axisValue) {
            return static_cast<int16_t>((axisValue >> 1U));
        }

        static inline int16_t scaleThrottleValue(int32_t axisValue) {
            int16_t val = static_cast<uint16_t>((axisValue >> 2U) + ConstantValues::NEUTRAL_AXIS_VAL);
            if (val > 255) {
                val -= 1;
            }
            return val;
        }
                
        static int32_t applyDeadzone(int32_t axis);
        
        static inline DroneControlPacket m_currentPacket;
        static inline DroneControlPacket m_sharedPacket;
        static inline std::mutex m_packetMutex;
        
        static inline esp_now_peer_info_t m_peer;
        
    public:
        RemoteControl() = delete;
        
        static DroneControlPacket getAndClearPacket();
        
        static void calculateCRC(DroneControlPacket* dronePacket);
        
        static void sendPacket(DroneControlPacket *packet);        
        
        /*
        This function initiates both wifi and esp_now protocol
        */
        static void initRemoteConnection();

        static void initLed();

        static void handlePadData(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad);

        static void padDisconnected();

        static void padReady();
};

#endif

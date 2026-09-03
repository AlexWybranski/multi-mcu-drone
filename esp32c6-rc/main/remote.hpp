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
}

namespace ConstantValues {
    constexpr int32_t DEAD_ZONE = 50;
    constexpr int32_t NEUTRAL_AXIS_VAL = 128U;

    constexpr size_t MAC_LENGTH = 6U; 
    constexpr std::array<uint8_t, MAC_LENGTH> RECEIVER_MAC = {0x9C, 0x13, 0x9E, 0xB5, 0x19, 0xC0};

    constexpr uint32_t PACKET_DATA_SIZE = 4U;
    constexpr size_t PACKET_SIZE = sizeof(DroneControlPacket);
    
    constexpr uint8_t N_THROTTLE = 128U;
    constexpr uint8_t N_ROLL = 128U;
    constexpr uint8_t N_PITCH = 128U;
    constexpr uint8_t N_BUTTON_REG = 0U;

    constexpr std::array<uint8_t, PACKET_DATA_SIZE> NEUTRAL_DATA{N_THROTTLE, N_ROLL, N_PITCH, N_BUTTON_REG};

    constexpr uint32_t N_CRC_CALC_VALUE = 0U;
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
        static inline uint8_t scaleAxisValue(int32_t axisValue) {
            return static_cast<uint8_t>((axisValue / 4) + ConstantValues::NEUTRAL_AXIS_VAL);
        }
                
        static int32_t applyDeadzone(int32_t axis);
        
        static inline DroneControlPacket m_currentPacket;
        static inline DroneControlPacket m_sharedPacket;
        static inline std::mutex m_packetMutex;
        
        static inline uint32_t N_PACKET_CRC;
        
        static inline esp_now_peer_info_t m_peer;

        public:
        RemoteControl() = delete;

        static void handlePadData(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad);

        static DroneControlPacket getAndClearPacket();
        
        static void calculateCRC(DroneControlPacket* dronePacket);
        
        static void sendPacket(DroneControlPacket *packet);        

        /*
            Use this function only once
        */
        static inline void initNeutralCRC() {
            N_PACKET_CRC = esp_rom_crc32_le(ConstantValues::N_CRC_CALC_VALUE, ConstantValues::NEUTRAL_DATA.data(), ConstantValues::PACKET_DATA_SIZE);
        }

        /*
            This function initiates both wifi and esp_now protocol
        */
        static void initRemoteConnection();
};

#endif

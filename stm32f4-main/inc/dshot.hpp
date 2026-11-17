#ifndef DSHOT_HPP
#define DSHOT_HPP

#include <cstdint>
#include <array>
#include <cmath>

#include "tim.hpp"

namespace DSHOT {
    // DSHOT PACKET STRUCTURE
    // type: uint16_t
    // bits 15:5 - throttle
    // bit 4 - telemetry request
    // bits 3:0 - crc value

    constexpr uint32_t THROTTLE_SHIFT = 5U;
    constexpr uint32_t TELEMETRY_SHIFT = 4U;
    constexpr uint32_t CRC_SHIFT = 0U;
    constexpr std::size_t DSHOT_BUFFER_SIZE = 18U;

    constexpr uint16_t T1H = static_cast<uint16_t>((TIM_SETUP::ARR_VAL+1U) * 0.75f);
    constexpr uint16_t T0H = static_cast<uint16_t>((TIM_SETUP::ARR_VAL+1U) * 0.375f);
}

class DShot {
    private:
        std::array<uint16_t, DSHOT::DSHOT_BUFFER_SIZE> dShotBuffer{};

        using DmaWritingFunc = void(*)(const uint16_t* src, size_t size);
        DmaWritingFunc writeFunction;

        using DmaEnableStreamFunc = void(*)(void);
        DmaEnableStreamFunc enableDmaStream;

    public:
        DShot() = default;
        ~DShot() = default;

        void assembleDshotBuffer(uint16_t duty, bool telemetry);

        void registerWritingFunc(DmaWritingFunc func);

        void registerStreamEnableFunc(DmaEnableStreamFunc func);

        void setupTransfer();

        void startTransfer();
};

#endif
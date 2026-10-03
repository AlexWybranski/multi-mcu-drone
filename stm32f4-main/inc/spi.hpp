#ifndef SPI_HPP
#define SPI_HPP
#include <cstdint>

extern "C" {
    #include "FreeRTOS.h" // IWYU pragma: keep
    #include "task.h"
}

#include "gpio.hpp"

namespace SPI_SETUP {
    constexpr uint32_t CR1_BR_VAL = (0b101U << 3U); // pclk/64 -> 48 MHz / 64 = 750 kHz
    constexpr uint32_t CR1_BR_HIGHSPEED_VAL = (0b010U << 3U); // pclk/8 -> 48 MHz / 8 = 6 mHz
    constexpr uint32_t CR1_SSM = (0b1U << 9U);
    constexpr uint32_t CR1_SSI = (0b1U << 8U);
    constexpr uint32_t CR1_CPHA = (0b1U << 0U);
    constexpr uint32_t CR1_CPOL = (0b1U << 1U);
    constexpr uint32_t CR1_MSTR = (0b1U << 2U);
    constexpr uint32_t CR1_SPE = (0b1U << 6U);

    constexpr uint32_t CR2_TXEIE = (0b1U << 7U);
    constexpr uint32_t CR2_RXNEIE = (0b1U << 6U);
    constexpr uint32_t CR2_ERRIE = (0b1U << 5U);
    constexpr uint32_t CR2_TXDMAEN = (0b1U << 1U);
    constexpr uint32_t CR2_RXDMAEN = (0b1U << 0U);

    constexpr uint32_t SR_BSY = (0b1U << 7U);
    constexpr uint32_t SR_OVR = (0b1U << 6U);
    constexpr uint32_t SR_TXE = (0b1U << 1U);
    constexpr uint32_t SR_RXNE = (0b1U << 0U);

}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct SPI_regs {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
};

class SpiHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        SPI_regs* const m_SPI; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

        GpioHandle* GPIO_ptr{nullptr};
        uint32_t m_CS_PIN{0};

    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit SpiHandle(uint32_t baseAddr, GpioHandle* GPIO_PORT_ptr) : m_SPI(reinterpret_cast<SPI_regs*>(baseAddr)), GPIO_ptr(GPIO_PORT_ptr) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~SpiHandle() = default;
        SpiHandle(const SpiHandle& other) = delete;
        SpiHandle& operator=(const SpiHandle& other) = delete;
        SpiHandle(SpiHandle&& other) = delete;
        SpiHandle& operator=(SpiHandle&& other) = delete;

        static SpiHandle* instance;

        /*
            SPI is being initialized to work on:
                - Full duplex
                - No CRC
                - 8-bit data frame format
                - Software CS management
                - 6 MHz - 750kHz for debug purposes
                - Master mode
                - Mode 3 (CPOL = 1 and CPHA = 1) - Needed by LSM6DS3TR-C IMU Sensor
                - DMA
        */
        void init(uint32_t CS_PIN_NUM);

        void DMAstartTransfer();

        void DMAendTransfer();

        void POLLread_write(const uint8_t* txBuff, uint8_t* rxBuff, std::size_t size, bool writeOnly);

        void setCsLow();

        void setCsHigh();

        volatile uint32_t* getDataRegAddr();

        void handleIRQ();
};

#endif
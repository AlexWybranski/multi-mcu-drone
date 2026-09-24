#ifndef SPI_HPP
#define SPI_HPP
#include <cstdint>

#include "gpio.hpp"

namespace SPI_constants {
    constexpr std::size_t TX_BUFFER_SIZE = 32;
    constexpr std::size_t RX_BUFFER_SIZE = 32;
}

namespace SPI_SETUP {
    constexpr uint32_t CR1_BR_VAL = 0b010U; // pclk/8 -> 48 MHz / 8 = 6 MHz
    constexpr uint32_t CR1_BR_SHIFT = 3U;
    constexpr uint32_t CR1_SSM_VAL = 0b1U;
    constexpr uint32_t CR1_SSM_SHIFT = 9U;
    constexpr uint32_t CR1_SSI_VAL = 0b1U;
    constexpr uint32_t CR1_SSI_SHIFT = 8U;
    constexpr uint32_t CR1_CPHA_VAL = 0b1U;
    constexpr uint32_t CR1_CPHA_SHIFT = 0;
    constexpr uint32_t CR1_CPOL_VAL = 0b1U;
    constexpr uint32_t CR1_CPOL_SHIFT = 1U;
    constexpr uint32_t CR1_MSTR_VAL = 0b1U;
    constexpr uint32_t CR1_MSTR_SHIFT = 2U;
    constexpr uint32_t CR1_SPE_VAL = 0b1U;
    constexpr uint32_t CR1_SPE_SHIFT = 6U;

    constexpr uint32_t CR2_TXEIE_RESET = 0b1U;
    constexpr uint32_t CR2_TXEIE_SHIFT = 7U;
    constexpr uint32_t CR2_RXNEIE_RESET = 0b1U;
    constexpr uint32_t CR2_RXNEIE_SHIFT = 6U;
    constexpr uint32_t CR2_ERRIE_RESET = 0b1U;
    constexpr uint32_t CR2_ERRIE_VAL = 0b1U;
    constexpr uint32_t CR2_ERRIE_SHIFT = 5U;

    constexpr uint32_t SR_OVR = (0b1U << 6U);
    constexpr uint32_t SR_TXE = (0b1U << 1U);
    constexpr uint32_t SR_RXNE = (0b1U << 0U);

    constexpr uint32_t CR2_TXEIE = (0b1U << 7U);
    constexpr uint32_t CR2_RXNEIE = (0b1U << 6U);
    constexpr uint32_t CR2_ERRIE = (0b1U << 5U);
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

        uint8_t* m_txBuff{};
        volatile std::size_t m_txIndex{0};
        uint8_t* m_rxBuff{};
        volatile std::size_t m_rxIndex{0};
        std::size_t m_size{0};
        volatile std::size_t m_byteCounter{0};
        bool m_writeOnly{false};
        const uint32_t m_dummyByte = 0xFF;

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
                - 6 MHz
                - Master mode
                - Mode 3 (CPOL = 1 and CPHA = 1) - Needed by LSM6DS3TR-C IMU Sensor

                - TX interrupts
                - RX interrupts
                - Error interrupts
        */
        void init(uint32_t CS_PIN_NUM);

        void read_write(uint8_t* txBuff, uint8_t* rxBuff, std::size_t size, bool writeOnly);

        void setCsHigh();

        volatile uint32_t* getDataRegAddr();

        void handleIRQ();
};

#endif
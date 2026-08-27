#ifndef SPI_HPP
#define SPI_HPP
#include <cstdint>

struct SPI_regs {
    volatile uint32_t CR1;
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
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit SpiHandle(uint32_t baseAddr) : m_SPI(reinterpret_cast<SPI_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~SpiHandle() = default;
        SpiHandle(const SpiHandle& other) = delete;
        SpiHandle& operator=(const SpiHandle& other) = delete;
        SpiHandle(SpiHandle&& other) = delete;
        SpiHandle& operator=(SpiHandle&& other) = delete;
};

#endif
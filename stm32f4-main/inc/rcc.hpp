#ifndef RCC_HPP
#define RCC_HPP
#include <cstdint>

struct RCC_regs {
    volatile uint32_t   CR;
    volatile uint32_t   PLLCFGR;
    volatile uint32_t   CFGR;
    volatile uint32_t   CIR;
    volatile uint32_t   AHB1RSTR;
    volatile uint32_t   AHB2RSTR;

    const uint32_t      RESERVED;
    const uint32_t      RESERVED1;

    volatile uint32_t   APB1RSTR;
    volatile uint32_t   APB2RSTR;

    const uint32_t      RESERVER2;
    const uint32_t      RESERVED3;

    volatile uint32_t   AHB1ENR;
    volatile uint32_t   AHB2ENR;

    const uint32_t      RESERVER4;
    const uint32_t      RESERVED5;

    volatile uint32_t   APB1ENR;
    volatile uint32_t   APB2ENR;

    const uint32_t      RESERVED6;
    const uint32_t      RESERVED7;

    volatile uint32_t   AHB1LPENR;
    volatile uint32_t   AHB2LPENR;

    const uint32_t      RESERVED8;
    const uint32_t      RESERVED9;

    volatile uint32_t   APB1LPENR;
    volatile uint32_t   APB2LPENR;

    const uint32_t      RESERVED10;
    const uint32_t      RESERVED11;

    volatile uint32_t   BDCR;
    volatile uint32_t   CSR;

    const uint32_t      RESERVED12;
    const uint32_t      RESERVED13;

    volatile uint32_t   SSCGR;
    volatile uint32_t   PLLI2SCFGR;

    const uint32_t      RESERVED14;

    volatile uint32_t   DCKCFGR;
};

class RccHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        RCC_regs* const m_RCC; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit RccHandle(uint32_t baseAddr) : m_RCC(reinterpret_cast<RCC_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~RccHandle() = default;
        RccHandle(const RccHandle& other) = delete;
        RccHandle& operator=(const RccHandle& other) = delete;
        RccHandle(RccHandle&& other) = delete;
        RccHandle& operator=(RccHandle&& other) = delete;

        constexpr static uint32_t calculatePllCfgrValue();

        /*
            This function is supposed to be executed before any other to ensure proper frequency for all peripherals
        */
        void setClock();

        void enableAHB1PeripheralClock(uint32_t peripheralBit);

        void enableAPB1PeripheralClock(uint32_t peripheralBit);

        void enableTim1Clock();
};

#endif
#ifndef RCC_HPP
#define RCC_HPP
#include <cstdint>

namespace RCC_SETUP {
    constexpr uint32_t FLASH_BASEADDR = 0x40023C00;
    constexpr uint32_t FLASH_ACR_LATENCY_SET = (0b0001U << 0U);
    constexpr uint32_t FLASH_ACR_LATENCY_RESET = ~(0b1111U << 0U);

    constexpr uint32_t CFGR_SW_RESET = (0b11U << 0U);
    constexpr uint32_t CFGR_SW_SET = (0b10U << 0U);
    constexpr uint32_t CFGR_SWS_DESIRED_VAL = (0b10U << 2U);
    constexpr uint32_t CFGR_PPRE1_RESET = (0b111U << 10U);
    constexpr uint32_t CFGR_PPRE2_RESET = (0b111U << 13U);

    constexpr uint32_t CR_HSEON = (0b1U << 16U);
    constexpr uint32_t CR_HSERDY = (0b1U << 17U);

    constexpr uint32_t CR_PLLON = (0b1U << 24U);  
    constexpr uint32_t CR_PLLRDY = (0b1U << 25U);  
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct RCC_regs {
    volatile uint32_t   CR;
    volatile uint32_t   PLLCFGR;
    volatile uint32_t   CFGR;
    volatile uint32_t   CIR;
    volatile uint32_t   AHB1RSTR;
    volatile uint32_t   AHB2RSTR;

    const uint32_t      RESERVED; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED1; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   APB1RSTR;
    volatile uint32_t   APB2RSTR;

    const uint32_t      RESERVER2; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED3; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   AHB1ENR;
    volatile uint32_t   AHB2ENR;

    const uint32_t      RESERVER4; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED5; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   APB1ENR;
    volatile uint32_t   APB2ENR;

    const uint32_t      RESERVED6; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED7; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   AHB1LPENR;
    volatile uint32_t   AHB2LPENR;

    const uint32_t      RESERVED8; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED9; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   APB1LPENR;
    volatile uint32_t   APB2LPENR;

    const uint32_t      RESERVED10; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED11; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   BDCR;
    volatile uint32_t   CSR;

    const uint32_t      RESERVED12; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    const uint32_t      RESERVED13; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   SSCGR;
    volatile uint32_t   PLLI2SCFGR;

    const uint32_t      RESERVED14; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    volatile uint32_t   DCKCFGR;
};

class RccHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        RCC_regs* const m_RCC; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

        [[nodiscard("returned value must be assigned to variable")]]static consteval uint32_t calculatePllCfgrValue();
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit RccHandle(uint32_t baseAddr) : m_RCC(reinterpret_cast<RCC_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~RccHandle() = default;
        RccHandle(const RccHandle& other) = delete;
        RccHandle& operator=(const RccHandle& other) = delete;
        RccHandle(RccHandle&& other) = delete;
        RccHandle& operator=(RccHandle&& other) = delete;

        /*
            This function is supposed to be executed before any other to ensure proper frequency for all peripherals
            Sets sysclk to PLL sourced by HSE, sysclk works on 48MHz
        */
        void setClock();

        void enableAHB1PeripheralClock(uint32_t peripheralBit);

        void enableAPB1PeripheralClock(uint32_t peripheralBit);

        void enableAPB2PeripheralClock(uint32_t peripheralBit);
};

#endif
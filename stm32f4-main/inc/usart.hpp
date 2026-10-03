#ifndef USART_HPP
#define USART_HPP
#include <cstdint>

namespace USART_SETUP {
    constexpr uint32_t REG_RESET_VAL = 0U;

    constexpr uint32_t CR1_UE = (0b1U << 13U);
    constexpr uint32_t CR1_RE = (0b1U << 2U);

    constexpr uint32_t CR3_DMAR = (0b1U << 6U);
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct USART_regs {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
};

class UsartHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        USART_regs* const m_USART; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

        [[nodiscard("returned value must be assigned to variable")]]static consteval uint32_t calculateBRRregValue();
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit UsartHandle(uint32_t baseAddr) : m_USART(reinterpret_cast<USART_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~UsartHandle() = default;
        UsartHandle(const UsartHandle& other) = delete;
        UsartHandle& operator=(const UsartHandle& other) = delete;
        UsartHandle(UsartHandle&& other) = delete;
        UsartHandle& operator=(UsartHandle&& other) = delete;

        volatile uint32_t* getDataRegAddr();

        void init();

        void handleIRQ();
};

#endif
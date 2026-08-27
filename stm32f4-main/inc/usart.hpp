#ifndef USART_HPP
#define USART_HPP
#include <cstdint>

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
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit UsartHandle(uint32_t baseAddr) : m_USART(reinterpret_cast<USART_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~UsartHandle() = default;
        UsartHandle(const UsartHandle& other) = delete;
        UsartHandle& operator=(const UsartHandle& other) = delete;
        UsartHandle(UsartHandle&& other) = delete;
        UsartHandle& operator=(UsartHandle&& other) = delete;
};

#endif
#ifndef TIM_HPP
#define TIM_HPP
#include <cstdint>
/*
    This class is supposed to be used with TIM1 (Advanced Control Timer)
*/
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct TIM1_regs {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
};

class Tim1Handle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        TIM1_regs* const m_TIM; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit Tim1Handle(uint32_t baseAddr) : m_TIM(reinterpret_cast<TIM1_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~Tim1Handle() = default;
        Tim1Handle(const Tim1Handle& other) = delete;
        Tim1Handle& operator=(const Tim1Handle& other) = delete;
        Tim1Handle(Tim1Handle&& other) = delete;
        Tim1Handle& operator=(Tim1Handle&& other) = delete;
};

#endif
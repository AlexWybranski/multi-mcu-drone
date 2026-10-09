#ifndef TIM_HPP
#define TIM_HPP
#include <cstdint>

namespace TIM_SETUP {
    constexpr uint32_t CR1_CMS_CENTER_MODE_1 = (0b01U << 5U);
    constexpr uint32_t CR1_CEN = (0b01U << 0U);

    constexpr uint32_t CCMR1_CH1_PRELOAD_ENABLE = (0b1U << 3U);
    constexpr uint32_t CCMR1_CH2_PRELOAD_ENABLE = (0b1U << 11U);
    constexpr uint32_t CCMR2_CH3_PRELOAD_ENABLE = (0b1U << 3U);
    constexpr uint32_t CCMR2_CH4_PRELOAD_ENABLE = (0b1U << 11U);
    constexpr uint32_t CCMR1_CH1_PWM_MODE_1 = (0b110U << 4U);
    constexpr uint32_t CCMR1_CH2_PWM_MODE_1 = (0b110U << 12U);
    constexpr uint32_t CCMR2_CH3_PWM_MODE_1 = (0b110U << 4U);
    constexpr uint32_t CCMR2_CH4_PWM_MODE_1 = (0b110U << 12U);

    constexpr uint32_t CCER_CH1_ENABLE = (0b1U << 0U);
    constexpr uint32_t CCER_CH2_ENABLE = (0b1U << 4U);
    constexpr uint32_t CCER_CH3_ENABLE = (0b1U << 8U);
    constexpr uint32_t CCER_CH4_ENABLE = (0b1U << 12U);

    constexpr uint32_t BDTR_MOE = (0b1U << 15U);

    constexpr uint32_t PSC_VAL = 0U;

    constexpr uint32_t ARR_VAL = 500U;

    constexpr uint16_t MAX_DUTY = static_cast<uint16_t>(ARR_VAL);
    constexpr uint32_t MAX_CHANNEL = 4;
}

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

/*
    PWM spec:
    - Center aligned wave
    - ARR - Max counter value
    - PSC - 
*/

/*
    This class is supposed to be used with TIM1 (Advanced Control Timer) only
*/
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

        void init();

        void setDutyCh1(int16_t duty);
        void setDutyCh2(int16_t duty);
        void setDutyCh3(int16_t duty);
        void setDutyCh4(int16_t duty);
};

#endif
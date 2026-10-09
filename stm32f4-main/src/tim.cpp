#include "tim.hpp"

void Tim1Handle::init() {
    using namespace TIM_SETUP;

    uint32_t CR1_MASK = CR1_CMS_CENTER_MODE_1;
    uint32_t CCMR1_MASK = (CCMR1_CH1_PRELOAD_ENABLE | CCMR1_CH1_PWM_MODE_1 | CCMR1_CH2_PRELOAD_ENABLE | CCMR1_CH2_PWM_MODE_1);
    uint32_t CCMR2_MASK = (CCMR2_CH3_PRELOAD_ENABLE | CCMR2_CH3_PWM_MODE_1 | CCMR2_CH4_PRELOAD_ENABLE | CCMR2_CH4_PWM_MODE_1);
    uint32_t CCER_MASK = (CCER_CH1_ENABLE | CCER_CH2_ENABLE | CCER_CH3_ENABLE | CCER_CH4_ENABLE);
    uint32_t BDTR_MASK = BDTR_MOE;

    m_TIM->CR1 = CR1_MASK;
    m_TIM->CCMR1 = CCMR1_MASK;
    m_TIM->CCMR2 = CCMR2_MASK;
    m_TIM->CCER = CCER_MASK;
    m_TIM->BDTR = BDTR_MASK;
    m_TIM->PSC = 0U;
    m_TIM->ARR = ARR_VAL;

    m_TIM->CR1 |= CR1_CEN;
}

void Tim1Handle::setDutyCh1(int16_t duty) {
    if (duty > TIM_SETUP::MAX_DUTY) {
        duty = TIM_SETUP::MAX_DUTY;
    }
    if (duty < 0)
    {
        duty = 0;
    }
    
    m_TIM->CCR1 = static_cast<uint16_t>(duty);
}
void Tim1Handle::setDutyCh2(int16_t duty) {
    if (duty > TIM_SETUP::MAX_DUTY) {
        duty = TIM_SETUP::MAX_DUTY;
    }
    if (duty < 0)
    {
        duty = 0;
    }
    
    m_TIM->CCR2 = static_cast<uint16_t>(duty);
}
void Tim1Handle::setDutyCh3(int16_t duty) {
    if (duty > TIM_SETUP::MAX_DUTY) {
        duty = TIM_SETUP::MAX_DUTY;
    }
    if (duty < 0)
    {
        duty = 0;
    }
    
    m_TIM->CCR3 = static_cast<uint16_t>(duty);
}
void Tim1Handle::setDutyCh4(int16_t duty) {
    if (duty > TIM_SETUP::MAX_DUTY) {
        duty = TIM_SETUP::MAX_DUTY;
    }
    if (duty < 0)
    {
        duty = 0;
    }
    
    m_TIM->CCR4 = static_cast<uint16_t>(duty);
}
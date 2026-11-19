#include "tim.hpp"

void Tim1Handle::init() {
    using namespace TIM_SETUP;

    uint32_t CR1_MASK = CR1_ARPE;
    uint32_t CCMR1_MASK = (CCMR1_CH1_PRELOAD_ENABLE | CCMR1_CH1_PWM_MODE_1 | CCMR1_CH2_PRELOAD_ENABLE | CCMR1_CH2_PWM_MODE_1);
    uint32_t CCMR2_MASK = (CCMR2_CH3_PRELOAD_ENABLE | CCMR2_CH3_PWM_MODE_1 | CCMR2_CH4_PRELOAD_ENABLE | CCMR2_CH4_PWM_MODE_1);
    uint32_t CCER_MASK = (CCER_CH1_ENABLE | CCER_CH2_ENABLE | CCER_CH3_ENABLE | CCER_CH4_ENABLE);
    uint32_t BDTR_MASK = BDTR_MOE;

    m_TIM->CR1 = CR1_MASK;
    m_TIM->CCMR1 = CCMR1_MASK;
    m_TIM->CCMR2 = CCMR2_MASK;
    m_TIM->CCER = CCER_MASK;
    m_TIM->BDTR = BDTR_MASK;
    m_TIM->PSC = PSC_VAL;
    m_TIM->ARR = ARR_VAL;
}

void Tim1Handle::disableDmaRequests() {
    m_TIM->DIER &= ~(0b1111U << 9U);
}

void Tim1Handle::enableDmaRequests() {
    m_TIM->DIER |= (0b1111U << 9U);
}

void Tim1Handle::disableTimer() {
    m_TIM->CR1 &= ~TIM_SETUP::CR1_CEN;
}

void Tim1Handle::enableTimer() {
    m_TIM->CR1 |= TIM_SETUP::CR1_CEN;
}

void Tim1Handle::disableTimDShot() {
    disableTimer();
    disableDmaRequests();

    m_TIM->CCR1 = 0U;
    m_TIM->CCR2 = 0U;
    m_TIM->CCR3 = 0U;
    m_TIM->CCR4 = 0U;

    m_TIM->CNT = 0U;
    m_TIM->EGR = (0b1U << 0U);
    m_TIM->SR = 0U;
}

void Tim1Handle::enableTimDShot() {
    enableTimer();
    enableDmaRequests();
}

volatile uint32_t* Tim1Handle::getCh1Address() {
    return &m_TIM->CCR1;
} 
volatile uint32_t* Tim1Handle::getCh2Address() {
    return &m_TIM->CCR2;
} 
volatile uint32_t* Tim1Handle::getCh3Address() {
    return &m_TIM->CCR3;
} 
volatile uint32_t* Tim1Handle::getCh4Address() {
    return &m_TIM->CCR4;
} 
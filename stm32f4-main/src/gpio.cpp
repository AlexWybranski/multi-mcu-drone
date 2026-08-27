#include "gpio.hpp"

void GpioHandle::setPinMode(GpioHandle::Mode mode, uint32_t pinNum) {
    constexpr uint32_t resetValue = 0b11U;

    auto setValue = static_cast<uint32_t>(mode);
    uint32_t shift = pinNum * 2U;

    m_GPIO->MODER &= ~(resetValue << shift);
    m_GPIO->MODER |= (setValue << shift);
}

void GpioHandle::setPinOutputType(bool isOpenDrain, uint32_t pinNum) {
    constexpr uint32_t resetValue = 0b1U; 
    
    uint32_t shift = pinNum;
    
    m_GPIO->OTYPER &= ~(resetValue << shift);
    
    if(isOpenDrain) {
        uint32_t setValue = 0b1U;
        m_GPIO->OTYPER |= (setValue << shift);
    }
}

void GpioHandle::setPinOutputSpeed(GpioHandle::Speed speed, uint32_t pinNum) {
    constexpr uint32_t resetValue = 0b11U;

    auto setValue = static_cast<uint32_t>(speed);
    uint32_t shift = pinNum * 2U;

    m_GPIO->OSPEEDR &= ~(resetValue << shift);
    m_GPIO->OSPEEDR |= (setValue << shift);
}

void GpioHandle::setPinPullupPulldown(GpioHandle::Pull pull, uint32_t pinNum) {
    constexpr uint32_t resetValue = 0b11U;

    auto setValue = static_cast<uint32_t>(pull);
    uint32_t shift = pinNum * 2U;

    m_GPIO->PUPDR &= ~(resetValue << shift);
    m_GPIO->PUPDR |= (setValue << shift);
}

void GpioHandle::setPinState(bool state, uint32_t pinNum) {
    constexpr uint32_t BSRR_RESET_OFFSET = 16U;

    uint32_t setValue = 0b1U;
    uint32_t shift = pinNum;
    if(!state) {
        shift += BSRR_RESET_OFFSET;
    }
    m_GPIO->BSRR = (setValue << shift);
}

void GpioHandle::setPinAlternateFunction(GpioHandle::Func func, uint32_t pinNum) {
    constexpr uint32_t AFR_PINS_PER_REG = 8U;
    constexpr uint32_t resetValue = 0b1111U;

    auto setValue = static_cast<uint32_t>(func);
    if(pinNum >= AFR_PINS_PER_REG) {
        uint32_t shift = (pinNum % AFR_PINS_PER_REG) * 4U;
        m_GPIO->AFRH &= ~(resetValue << shift);
        m_GPIO->AFRH |= (setValue << shift);
    } else {
        uint32_t shift = pinNum * 4U;
        m_GPIO->AFRL &= ~(resetValue << shift);
        m_GPIO->AFRL |= (setValue << shift);
    }
}
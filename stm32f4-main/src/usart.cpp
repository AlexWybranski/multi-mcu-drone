#include "usart.hpp"

namespace USART_SETUP {
    constexpr uint32_t REG_RESET_VAL = 0U;

    constexpr uint32_t CR1_UE = (0b1U << 13U);
    constexpr uint32_t CR1_RE = (0b1U << 2U);

    constexpr uint32_t CR3_DMAR = (0b1U << 6U);
}

UsartHandle* UsartHandle::instance = nullptr;

/*
    This function ignores heavy math from F4 Reference manual, and only uses ready DIV_Mantissa and DIV_Fraction values

    Calculated values are for 115200 Baud rate with 48 MHz clock on APB2 bus
*/
consteval uint32_t UsartHandle::calculateBRRregValue() {
    uint32_t val{0};

    const uint32_t mantissa = 26U;
    const uint32_t fraction = 1U;
    
    uint32_t mantissa_reg = (mantissa << 4U);
    uint32_t fraction_reg = (fraction << 0U);

    val |= (mantissa_reg | fraction_reg);

    return val;
}

void UsartHandle::init() {
    using namespace USART_SETUP;

    m_USART->CR1 = REG_RESET_VAL;
    m_USART->CR1 |= CR1_RE;

    m_USART->CR3 = REG_RESET_VAL;
    m_USART->CR3 |= CR3_DMAR;

    constexpr uint32_t brrValue = calculateBRRregValue();

    m_USART->BRR = brrValue;
    
    m_USART->CR1 |= CR1_UE;
}

volatile uint32_t* UsartHandle::getDataRegAddr() {
    return &m_USART->DR;
}

void UsartHandle::handleIRQ() {
    [[maybe_unused]]uint32_t dummySR = m_USART->SR;
    [[maybe_unused]]uint32_t dummyDR = m_USART->DR;

    return;
}
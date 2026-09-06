#include "usart.hpp"

namespace USART_SETUP {
    constexpr uint32_t REG_RESET_VAL = 0U;

    constexpr uint32_t CR1_UE = (0b1U << 13U);
    constexpr uint32_t CR1_RE = (0b1U << 2U);

    constexpr uint32_t CR3_DMAR = (0b1U << 6U);
}

/*
    This function ignores heavy math from F4 Reference manual, and only uses ready DIV_Mantissa and DIV_Fraction values

    Calculated values are for 115200 Baud rate with 48 MHz clock on APB2 bus
*/
constexpr uint32_t UsartHandle::calculateBRRregValue() {
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

    m_USART->BRR = calculateBRRregValue();

    m_USART->CR1 |= CR1_UE;
}
#include "usart.hpp"

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

void UsartHandle::enableIdleLineInterrupt() {
    m_USART->CR1 &= ~USART_SETUP::CR1_UE;

    for (int i = 0; i < 10; ++i) {
        // Wait for usart peripheral to disable
    }

    m_USART->CR1 |= USART_SETUP::CR1_IDLEIE;

    m_USART->CR1 |= USART_SETUP::CR1_UE;
}

void UsartHandle::setDmaFunc(DmaFunc func) {
    dmaIsWholePackageReceived = func;
}

void UsartHandle::setDmaResetFunc(DmaResetFunc func) {
    dmaResetStream = func;
}

void UsartHandle::handleIRQ() {
    using namespace USART_SETUP;
    [[maybe_unused]]uint32_t srReg = m_USART->SR;

    if ((srReg & SR_ORE) && (m_USART->CR3 & CR3_EIE)) {
        [[maybe_unused]]uint32_t dummyDR = m_USART->DR;
    }

    /*
        this is only needed to ensure synchronization between stm and esc

        if there is no synchronization this irq will reset dma stream between telemetry packages from esc
    */
    if ((srReg & SR_IDLE) && (m_USART->CR1 & CR1_IDLEIE)) {
        if (!dmaIsWholePackageReceived()) {
            dmaResetStream();
        }
        // else dma irq will handle passing adress to task and no action is needed
    }

    return;
}
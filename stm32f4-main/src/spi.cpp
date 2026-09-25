#include "spi.hpp"

void SpiHandle::init(uint32_t CS_PIN_NUM) {
    using namespace SPI_SETUP;
    
    SpiHandle::m_CS_PIN = CS_PIN_NUM;

    uint32_t cr1RegMask = 0;
    cr1RegMask |=   (
                        (CR1_BR_VAL << CR1_BR_SHIFT) | 
                        (CR1_SSM_VAL << CR1_SSM_SHIFT) |
                        (CR1_SSI_VAL << CR1_SSI_SHIFT) |
                        (CR1_MSTR_VAL << CR1_MSTR_SHIFT) |
                        (CR1_CPOL_VAL << CR1_CPOL_SHIFT) |
                        (CR1_CPHA_VAL << CR1_CPHA_SHIFT)
                    );
                    
    //Reset cr2 bits that will be used
    m_SPI->CR2 &=   ~(
                        (CR2_TXEIE_RESET << CR2_TXEIE_SHIFT) |
                        (CR2_RXNEIE_RESET << CR2_RXNEIE_SHIFT) |
                        (CR2_ERRIE_RESET << CR2_ERRIE_SHIFT)
                    );

    m_SPI->CR1 = cr1RegMask;
                    
    //Enable error interrupts
    m_SPI->CR2 |= (CR2_ERRIE_VAL << CR2_ERRIE_SHIFT);

    GPIO_ptr->setPinMode(GpioHandle::Mode::output, CS_PIN_NUM);
    GPIO_ptr->setPinOutputSpeed(GpioHandle::Speed::medium, CS_PIN_NUM);
    GPIO_ptr->setPinPullupPulldown(GpioHandle::Pull::pullup, CS_PIN_NUM);
    GPIO_ptr->setPinState(true, CS_PIN_NUM);

    m_SPI->CR1 |= (CR1_SPE_VAL << CR1_SPE_SHIFT);
}

void SpiHandle::DMAread_write() {
    using namespace SPI_SETUP;

    if (GPIO_ptr == nullptr || m_CS_PIN == 0) {
        return;
    }

    GPIO_ptr->setPinState(false, m_CS_PIN);

    //write buffer and wait
}

void SpiHandle::POLLread_write(uint8_t* txBuff, uint8_t* rxBuff, std::size_t size, bool writeOnly) {
    using namespace SPI_SETUP;

    uint32_t txIndex = 0;
    uint32_t rxIndex = 0;

    if (GPIO_ptr == nullptr || m_CS_PIN == 0) {
        return;
    }

    m_SPI->CR1 |= (CR1_SPE_VAL << CR1_SPE_SHIFT);

    GPIO_ptr->setPinState(false, m_CS_PIN);

    while (txIndex < size) {
        if ((m_SPI->SR & SR_RXNE) && !writeOnly) {
            rxBuff[rxIndex] = static_cast<uint8_t>(m_SPI->DR);
            rxIndex++;
        }
        if (m_SPI->SR & SR_TXE) {
            m_SPI->DR = static_cast<uint32_t>(txBuff[txIndex]);
            txIndex++;
        }
    }

    while (m_SPI->SR & SR_BSY) {}

    m_SPI->CR1 &= ~(CR1_SPE_VAL << CR1_SPE_SHIFT);

    GPIO_ptr->setPinState(true, m_CS_PIN);
}

void SpiHandle::setCsHigh() {
    GPIO_ptr->setPinState(true, m_CS_PIN);
}

volatile uint32_t* SpiHandle::getDataRegAddr() {
    return &m_SPI->DR;
}

void SpiHandle::handleIRQ() {
    using namespace SPI_SETUP;

    if((m_SPI->SR & SR_OVR) && (m_SPI->CR2 & CR2_ERRIE)) {
        [[maybe_unused]]uint32_t dummy = m_SPI->DR;
        [[maybe_unused]]uint32_t status = m_SPI->SR;

        GPIO_ptr->setPinState(true, m_CS_PIN);
    }
}
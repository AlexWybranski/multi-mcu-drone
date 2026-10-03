#include "spi.hpp"

void SpiHandle::init(uint32_t CS_PIN_NUM) {
    using namespace SPI_SETUP;
    
    SpiHandle::m_CS_PIN = CS_PIN_NUM;

    uint32_t cr1RegMask = 0;
    cr1RegMask |=   (
                        CR1_BR_VAL | 
                        CR1_SSM |
                        CR1_SSI |
                        CR1_MSTR |
                        CR1_CPOL |
                        CR1_CPHA
                    );
                    
    //Reset cr2 bits that will not be used
    m_SPI->CR2 &=   ~(
                        CR2_TXEIE |
                        CR2_RXNEIE |
                        CR2_ERRIE
                    );

    GPIO_ptr->setPinMode(GpioHandle::Mode::output, CS_PIN_NUM);
    GPIO_ptr->setPinOutputSpeed(GpioHandle::Speed::medium, CS_PIN_NUM);
    GPIO_ptr->setPinOutputType(false, CS_PIN_NUM);
    GPIO_ptr->setPinPullupPulldown(GpioHandle::Pull::pullup, CS_PIN_NUM);
    GPIO_ptr->setPinState(true, CS_PIN_NUM);
    
    m_SPI->CR1 = cr1RegMask;
                    
    m_SPI->CR1 |= CR1_SPE;
}

void SpiHandle::DMAstartTransfer() {
    using namespace SPI_SETUP;
    if (GPIO_ptr == nullptr || m_CS_PIN == 0) {
        return;
    }

    [[maybe_unused]]uint32_t dummySR = m_SPI->SR;
    [[maybe_unused]]uint32_t dummyDR = m_SPI->DR;

    if (m_SPI->SR & SR_OVR) {
        m_SPI->CR1 &= ~CR1_SPE;
        m_SPI->CR1 |= CR1_SPE;
    }

    m_SPI->CR2 |= (CR2_TXDMAEN | CR2_RXDMAEN);
    
    GPIO_ptr->setPinState(false, m_CS_PIN);
}

void SpiHandle::DMAendTransfer() {
    using namespace SPI_SETUP;

    m_SPI->CR2 &= ~(CR2_TXDMAEN | CR2_RXDMAEN);

    while (m_SPI->SR & SR_BSY) {}
    
    GPIO_ptr->setPinState(true, m_CS_PIN); 
}

void SpiHandle::POLLread_write(const uint8_t* txBuff, uint8_t* rxBuff, std::size_t size, bool writeOnly) {
    using namespace SPI_SETUP;

    uint32_t rxDummy = 0;

    if (GPIO_ptr == nullptr || m_CS_PIN == 0) {
        return;
    }

    GPIO_ptr->setPinState(false, m_CS_PIN);

    for (uint32_t i = 0; i < static_cast<uint32_t>(size); ++i) {
        while (!(m_SPI->SR & SR_TXE)) {}

        m_SPI->DR = static_cast<uint32_t>(txBuff[i]);

        while (!(m_SPI->SR & SR_RXNE)) {}

        rxDummy = m_SPI->DR;

        if (!writeOnly && rxBuff != nullptr) {
            rxBuff[i] = static_cast<uint8_t>(rxDummy);
        }
    }

    while (m_SPI->SR & SR_BSY) {}

    GPIO_ptr->setPinState(true, m_CS_PIN);
}

void SpiHandle::setCsLow() {
    GPIO_ptr->setPinState(false, m_CS_PIN);
}

void SpiHandle::setCsHigh() {
    GPIO_ptr->setPinState(true, m_CS_PIN);
}

volatile uint32_t* SpiHandle::getDataRegAddr() {
    return &m_SPI->DR;
}
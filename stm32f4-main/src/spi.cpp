#include "spi.hpp"

namespace SPI_SETUP {
    constexpr uint32_t CR1_BR_VAL = 0b010U; // pclk/8 -> 48 MHz / 8 = 6 MHz
    constexpr uint32_t CR1_BR_SHIFT = 3U;
    constexpr uint32_t CR1_SSM_VAL = 0b1U;
    constexpr uint32_t CR1_SSM_SHIFT = 9U;
    constexpr uint32_t CR1_SSI_VAL = 0b1U;
    constexpr uint32_t CR1_SSI_SHIFT = 8U;
    constexpr uint32_t CR1_CPHA_VAL = 0b1U;
    constexpr uint32_t CR1_CPHA_SHIFT = 0;
    constexpr uint32_t CR1_CPOL_VAL = 0b1U;
    constexpr uint32_t CR1_CPOL_SHIFT = 1U;
    constexpr uint32_t CR1_MSTR_VAL = 0b1U;
    constexpr uint32_t CR1_MSTR_SHIFT = 2U;
    constexpr uint32_t CR1_SPE_VAL = 0b1U;
    constexpr uint32_t CR1_SPE_SHIFT = 6U;

    constexpr uint32_t CR2_TXEIE_RESET = 0b1U;
    constexpr uint32_t CR2_TXEIE_SHIFT = 7U;
    constexpr uint32_t CR2_RXNEIE_RESET = 0b1U;
    constexpr uint32_t CR2_RXNEIE_SHIFT = 6U;
    constexpr uint32_t CR2_ERRIE_RESET = 0b1U;
    constexpr uint32_t CR2_ERRIE_VAL = 0b1U;
    constexpr uint32_t CR2_ERRIE_SHIFT = 5U;

    constexpr uint32_t SR_OVR = (0b1U << 6U);
    constexpr uint32_t SR_TXE = (0b1U << 1U);
    constexpr uint32_t SR_RXNE = (0b1U << 0U);

    constexpr uint32_t CR2_TXEIE = (0b1U << 7U);
    constexpr uint32_t CR2_RXNEIE = (0b1U << 6U);
    constexpr uint32_t CR2_ERRIE = (0b1U << 5U);
}

SpiHandle* SpiHandle::instance = nullptr;

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
    GPIO_ptr->setPinState(true, CS_PIN_NUM);

    m_SPI->CR1 |= (CR1_SPE_VAL << CR1_SPE_SHIFT);
}

void SpiHandle::read_write(uint8_t* txBuff, uint8_t* rxBuff, std::size_t size, bool writeOnly) {
    using namespace SPI_SETUP;

    if (GPIO_ptr == nullptr || m_CS_PIN == 0) {
        return;
    }

    m_rxBuff = rxBuff;
    m_txBuff = txBuff;
    m_size = size;
    m_writeOnly = writeOnly;
                            
    m_SPI->CR2 |= CR2_RXNEIE;
    m_SPI->CR2 |= CR2_TXEIE;

    GPIO_ptr->setPinState(false, m_CS_PIN);
}

void SpiHandle::setCsHigh() {
    GPIO_ptr->setPinState(true, m_CS_PIN);
}

void SpiHandle::handleIRQ() {
    using namespace SPI_SETUP;

    if((m_SPI->SR & SR_RXNE) && (m_SPI->CR2 & CR2_RXNEIE)) {
        if(!m_writeOnly && (m_byteCounter % 2 != 0)) {
            m_rxBuff[m_rxIndex] = static_cast<uint8_t>(m_SPI->DR);
            std::size_t helper = m_rxIndex;
            std::size_t byteHelper = m_byteCounter;
            helper++;
            byteHelper++;
            m_rxIndex = helper;
            m_byteCounter = byteHelper;
        } else {
            [[maybe_unused]]uint32_t dummy = m_SPI->DR;
            std::size_t byteHelper = m_byteCounter;
            byteHelper++;
            m_byteCounter = byteHelper;
        }
        if(m_byteCounter >= m_size) {
            m_SPI->CR2 &= ~CR2_RXNEIE;
            m_rxIndex = 0;
            m_byteCounter = 0;
            SpiHandle::setCsHigh();
        }
    }

    if((m_SPI->SR & SR_TXE) && (m_SPI->CR2 & CR2_TXEIE)) {
        m_SPI->DR = m_txBuff[m_txIndex];
        std::size_t helper = m_txIndex;
        helper++;
        m_txIndex = helper;
        if(m_txIndex >= m_size) {
            m_SPI->CR2 &= ~CR2_TXEIE;
            m_txIndex = 0;
        }
    }

    if((m_SPI->SR & SR_OVR) && (m_SPI->CR2 & CR2_ERRIE)) {
        [[maybe_unused]]uint32_t dummy = m_SPI->DR;
        [[maybe_unused]]uint32_t status = m_SPI->SR;

        m_SPI->CR2 &= ~CR2_RXNEIE;
        m_SPI->CR2 &= ~CR2_TXEIE;

        m_rxIndex = 0;
        m_txIndex = 0;
        m_byteCounter = 0;
        SpiHandle::setCsHigh();
    }
}
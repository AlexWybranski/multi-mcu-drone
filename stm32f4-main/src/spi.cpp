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
    constexpr uint32_t CR2_TXEIE_VAL = 0b1U;
    constexpr uint32_t CR2_TXEIE_SHIFT = 7U;
    constexpr uint32_t CR2_RXNEIE_RESET = 0b1U;
    constexpr uint32_t CR2_RXNEIE_VAL = 0b1U;
    constexpr uint32_t CR2_RXNEIE_SHIFT = 6U;
    constexpr uint32_t CR2_ERRIE_RESET = 0b1U;
    constexpr uint32_t CR2_ERRIE_VAL = 0b1U;
    constexpr uint32_t CR2_ERRIE_SHIFT = 5U;
}

void SpiHandle::init() {
    using namespace SPI_SETUP;
    
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
                    
    //Enable interrupts' bits
    m_SPI->CR2 |=   (
                        (CR2_TXEIE_VAL << CR2_TXEIE_SHIFT) |
                        (CR2_RXNEIE_VAL << CR2_RXNEIE_SHIFT) |
                        (CR2_ERRIE_VAL << CR2_ERRIE_SHIFT)
                    );

    m_SPI->CR1 |= (CR1_SPE_VAL << CR1_SPE_SHIFT);
}
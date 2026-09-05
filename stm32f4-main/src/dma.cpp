#include "dma.hpp"

#include <bit>

void DmaHandle::init(uint32_t* peripheral_reg_addr) {
    using namespace DMA_SETUP;

    m_DMA->S5CR &= ~ENABLE;

    while (m_DMA->S5CR & ENABLE) {}

    m_DMA->HIFCR &= CLEAR_TCF;

    m_DMA->S5CR &= CHANNEL_RESET;
    m_DMA->S5CR |= CHANNEL4_SET;

    m_DMA->S5CR |= DOUBLE_BUFFER_MODE;

    m_DMA->S5CR &= MSIZE_ONE_BYTE;
    m_DMA->S5CR &= PSIZE_ONE_BYTE;

    m_DMA->S5CR |= MINC_ON;
    m_DMA->S5CR &= PINC_OFF;

    m_DMA->S5CR &= PER_TO_MEM_DIRECTION;

    m_DMA->S5CR |= TCIE;

    m_DMA->S5NDTR = NDTR_VAL;

    m_DMA->S5PAR = std::bit_cast<uint32_t>(peripheral_reg_addr);

    m_DMA->S5M0AR = std::bit_cast<uint32_t>(bufferOne.data());
    m_DMA->S5M1AR = std::bit_cast<uint32_t>(bufferTwo.data());

    m_DMA->S5CR |= DMA_SETUP::ENABLE;
}
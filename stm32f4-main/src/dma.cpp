#include "dma.hpp"

#include <bit>

DmaHandle* DmaHandle::instance = nullptr;

// In this driver reinterpret_cast is needed to change address to uint32_t number in order to write value to register or to send it via FreeRTOS Notification register
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
/*
    @param peripheral_reg_addr - register from which dma will take data
    @param taskToNotify - task that will be notified and get pointer to buffer to read data from
*/
void DmaHandle::init(volatile uint32_t* peripheral_reg_addr, TaskHandle_t taskToNotify) {
    using namespace DMA_SETUP;

    m_taskToNotify = taskToNotify;

    m_DMA->S5CR &= ~ENABLE;

    while (m_DMA->S5CR & ENABLE) {}

    m_DMA->HIFCR |= CLEAR_TCF;

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
    
    m_DMA->S5M0AR = reinterpret_cast<uint32_t>(DmaHandle::bufferOne.data());
    m_DMA->S5M1AR = reinterpret_cast<uint32_t>(DmaHandle::bufferTwo.data());
    
    m_DMA->S5CR |= ENABLE;
}

void DmaHandle::handleIRQ() {
    using namespace DMA_SETUP;
    
    if (m_DMA->HISR & HISR_TCIF5) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (m_DMA->S5CR & CURRENT_TARGET) {
            xTaskNotifyFromISR
            (
                m_taskToNotify,
                reinterpret_cast<uint32_t>(bufferOne.data()),
                eSetValueWithOverwrite,
                &xHigherPriorityTaskWoken
            );
        } else {
            xTaskNotifyFromISR
            (
                m_taskToNotify,
                reinterpret_cast<uint32_t>(bufferTwo.data()),
                eSetValueWithOverwrite,
                &xHigherPriorityTaskWoken
            );
        }

        m_DMA->HIFCR |= CLEAR_TCF;

        //FreeRTOS macro, NOLINT used - non-user-code
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //NOLINT(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-type-cstyle-cast,hicpp-no-assembler)
    }
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
}
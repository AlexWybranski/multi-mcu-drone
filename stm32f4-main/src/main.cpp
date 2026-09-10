#include <cstdint>
#include <array>

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4.hpp"
#include "rcc.hpp"
#include "dma.hpp"
#include "tim.hpp"
#include "usart.hpp"
#include "spi.hpp"
#include "gpio.hpp"
#include "nvic.hpp"

// extern "C" block excluded from linter, due to freertos functions breaking the rules
extern "C" { //NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
    void _init(void) {}
    void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                        StackType_t **ppxIdleTaskStackBuffer,
                                        uint32_t *pulIdleTaskStackSize )
    {
        static StaticTask_t xIdleTaskTCB;
        static StackType_t uxIdleTaskStack[128];

        *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
        *ppxIdleTaskStackBuffer = uxIdleTaskStack;
        *pulIdleTaskStackSize = 128;
    }

    void DMA2_Stream5_IRQHandler() {
        DmaHandle::instance->handleIRQ();
    }

    void SPI2_IRQHandler() {
        SpiHandle::instance->handleIRQ();
    }
} //NOLINTEND

int main() {
    // alignas(4) std::array<uint8_t, DMA_SETUP::NDTR_VAL> bufferOne{};
    // alignas(4) std::array<uint8_t, DMA_SETUP::NDTR_VAL> bufferTwo{};
    static RccHandle rcc(PeripheralBaseAddr::RCC_BASEADDR);
    rcc.setClock();

    static Nvic nvic(PeripheralBaseAddr::NVIC_BASEADDR);
    nvic.enableIRQ(NVIC_IRQs::SPI2_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::SPI2_IRQ, NVIC_IRQs::SPI2_IRQ_Priority);
    nvic.enableIRQ(NVIC_IRQs::DMA2_Stream5_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::DMA2_Stream5_IRQ, NVIC_IRQs::DMA2_Stream5_IRQ_Priority);

    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOAEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOBEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::DMA2EN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::CRCEN);

    rcc.enableAPB1PeripheralClock(APB1_PERIPHERAL_BITS::SPI2EN);

    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::USART1EN);
    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::TIM1EN);
    
    static GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    static GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);

    static UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);

    static SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);

    static DmaHandle dma2(PeripheralBaseAddr::DMA2_BASEADDR);

    static Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);
}
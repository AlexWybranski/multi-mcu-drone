#include <cstdint>
#include <cstring>

extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}

#include "stm32f4.hpp"
#include "rcc.hpp"
#include "dma.hpp"
#include "tim.hpp"
#include "usart.hpp"
#include "spi.hpp"
#include "gpio.hpp"
#include "nvic.hpp"

#include "init.hpp"
#include "rtos_tasks.hpp"

// extern "C" block excluded from linter, due to freertos function breaking the rules
//NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
extern "C" {
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

    void USART1_IRQHandler() {
        UsartHandle::instance->handleIRQ();
    }
} //NOLINTEND(cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

int main() {
    static RccHandle rcc(PeripheralBaseAddr::RCC_BASEADDR);
    static Nvic nvic(PeripheralBaseAddr::NVIC_BASEADDR);

    static GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    static GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);

    static SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);
    
    static DmaHandle dma2(PeripheralBaseAddr::DMA2_BASEADDR);
    static UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);

    static Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);

    initClocks(rcc);
    initGpio(gpioA, gpioB);
    initIRQs(nvic);

    spi2.init(GPIOB_PINS::SPI2_SCS);

    initTasks();

    TaskHandle_t receiverTaskPtr = getReceiverTaskHandle();

    dma2.init(uart1.getDataRegAddr(), receiverTaskPtr);
    
    uart1.init();
    
    vTaskStartScheduler();

    while (1) {
    
    }

    return 0;
}
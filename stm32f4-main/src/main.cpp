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

namespace IRQ_Config {
    using IRQFunc = void(*)(void);

    inline DmaStreamHandle* dma1s3 = nullptr;
    inline DmaStreamHandle* dma2s5 = nullptr;
    inline UsartHandle* uart1 = nullptr;

    static constexpr std::array<IRQFunc, NVIC_IRQs::MAX_REGISTERED_IRQs> IRQ_table{
        []() {dma1s3->handleIRQ();},
        []() {dma2s5->handleIRQ();},
        []() {uart1->handleIRQ();},
    };
}

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

    void DMA1_Stream3_IRQHandler() {
        IRQ_Config::IRQ_table[0]();
    }

    void DMA2_Stream5_IRQHandler() {
        IRQ_Config::IRQ_table[1]();
    }

    void USART1_IRQHandler() {
        IRQ_Config::IRQ_table[2]();
    }
} //NOLINTEND(cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

int main() {
    static RccHandle rcc(PeripheralBaseAddr::RCC_BASEADDR);
    
    static GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    static GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);
    
    static DmaStreamHandle dma1s3(PeripheralBaseAddr::DMA1_BASEADDR, DMA_SETUP::STREAM_3);
    static DmaStreamHandle dma1s4(PeripheralBaseAddr::DMA1_BASEADDR, DMA_SETUP::STREAM_4);
    static DmaStreamHandle dma2s5(PeripheralBaseAddr::DMA2_BASEADDR, DMA_SETUP::STREAM_5);

    static SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);
    static UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);
    
    static Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);

    static Nvic nvic(PeripheralBaseAddr::NVIC_BASEADDR);

    IRQ_Config::uart1 = &uart1;
    IRQ_Config::dma1s3 = &dma1s3;
    IRQ_Config::dma2s5 = &dma2s5;

    initClocks(rcc);
    initGpio(gpioA, gpioB);
    initIRQs(nvic);

    spi2.init(GPIOB_PINS::SPI2_SCS);

    initTasks();

    TaskHandle_t receiverTaskPtr = getReceiverTaskHandle();

    dma1s3.init(spi2.getDataRegAddr(), nullptr);
    dma1s4.init(spi2.getDataRegAddr(), nullptr);
    dma2s5.init(uart1.getDataRegAddr(), receiverTaskPtr);
    
    uart1.init();
    
    vTaskStartScheduler();

    while (1) {
    
    }

    return 0;
}
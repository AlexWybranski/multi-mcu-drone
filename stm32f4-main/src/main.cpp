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

constexpr DmaStreamConfig dma1s3config { //SPI2 RX
    DmaChannel::ch0,
    DmaDirection::per_to_mem,
    DmaPriority::medium,
    DmaMode::single
};

constexpr DmaStreamConfig dma1s4config { //SPI2 TX
    DmaChannel::ch0,
    DmaDirection::mem_to_per,
    DmaPriority::low,
    DmaMode::single
};

constexpr DmaStreamConfig dma2s5config { //UART1 RX
    DmaChannel::ch4,
    DmaDirection::per_to_mem,
    DmaPriority::medium,
    DmaMode::double_buffer
};

//Pointers to objects are needed for IRQ_table, they have to be global and non-const because they are being initialized in main function
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace IRQ_Config {
    using IRQFunc = void(*)(void);

    inline DmaStreamHandle<PeripheralBaseAddr::DMA1_BASEADDR, DMA_SETUP::STREAM_3, dma1s3config, DMA_SETUP::SPI_NDTR_VAL>* volatile dma1s3 = nullptr;
    inline DmaStreamHandle<PeripheralBaseAddr::DMA2_BASEADDR, DMA_SETUP::STREAM_5, dma2s5config, DMA_SETUP::UART_NDTR_VAL>* volatile dma2s5 = nullptr;
    inline UsartHandle* uart1 = nullptr;
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

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

    void DMA1_Stream3_IRQHandler() { //SPI_RX
        if (IRQ_Config::dma1s3 != nullptr) {
            IRQ_Config::dma1s3->handleIRQ();
        }
    }

    void DMA2_Stream5_IRQHandler() { //UART_RX
        if (IRQ_Config::dma2s5 != nullptr) {
            IRQ_Config::dma2s5->handleIRQ();
        }
    }

    void USART1_IRQHandler() {
        if (IRQ_Config::uart1 != nullptr) {
            IRQ_Config::uart1->handleIRQ();
        }
    }
} //NOLINTEND(cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

int main() {
    static RccHandle rcc(PeripheralBaseAddr::RCC_BASEADDR);
    
    static GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    static GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);

    static DmaStreamHandle<PeripheralBaseAddr::DMA1_BASEADDR, DMA_SETUP::STREAM_3, dma1s3config, DMA_SETUP::SPI_NDTR_VAL> dma1s3_spirx; //SPI_RX
    static DmaStreamHandle<PeripheralBaseAddr::DMA1_BASEADDR, DMA_SETUP::STREAM_4, dma1s4config, DMA_SETUP::SPI_NDTR_VAL> dma1s4_spitx; //SPI_TX
    static DmaStreamHandle<PeripheralBaseAddr::DMA2_BASEADDR, DMA_SETUP::STREAM_5, dma2s5config, DMA_SETUP::UART_NDTR_VAL> dma2s5_uartrx; //UART_RX

    static SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);
    static UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);
    
    static Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);

    static Nvic nvic(PeripheralBaseAddr::NVIC_BASEADDR);

    IRQ_Config::uart1 = &uart1;
    IRQ_Config::dma1s3 = &dma1s3_spirx;
    IRQ_Config::dma2s5 = &dma2s5_uartrx;

    initClocks(rcc);
    initGpio(gpioA, gpioB);

    tim1.init();
    
    static imuTaskContext imuTaskCtx {
        []() {
            dma1s4_spitx.disableStream();
            dma1s3_spirx.disableStream();
            
            dma1s3_spirx.enableStream();
            dma1s4_spitx.enableStream();
        },
        &spi2
    };

    static enginesTaskContext enginesTaskCtx {
        &tim1
    };

    initTasks(&imuTaskCtx, &enginesTaskCtx);
    
    TaskHandle_t receiverTaskPtr = getReceiverTaskHandle();
    TaskHandle_t imuTaskPtr = getImuTaskHandle();
    
    dma1s3_spirx.init(spi2.getDataRegAddr(), imuTaskPtr);
    dma1s4_spitx.init(spi2.getDataRegAddr(), nullptr);
    dma2s5_uartrx.init(uart1.getDataRegAddr(), receiverTaskPtr);
    
    spi2.init(GPIOB_PINS::SPI2_SCS);
    uart1.init();

    initIRQs(nvic);
    
    vTaskStartScheduler();

    while (1) {
    
    }

    return 0;
}
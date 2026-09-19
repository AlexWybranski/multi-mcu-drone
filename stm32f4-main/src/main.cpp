#include <cstdint>
#include <array>
#include <cstring>

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
//NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers, cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
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
} //NOLINTEND

void imuTask(void* pvParameters);
void receiverTask(void* pvParameters);
void enginesTask(void* pvParameters);
void failsafeTask(void* pvParameters);

StackType_t imuTaskStack[256];
StackType_t receiverTaskStack[256];
StackType_t enginesTaskStack[256];
StackType_t failsafeTaskStack[256];

StaticTask_t imuTaskBuffer;
StaticTask_t receiverTaskBuffer;
StaticTask_t enginesTaskBuffer;
StaticTask_t failsafeTaskBuffer;

TaskHandle_t imuTaskHandle = nullptr;
TaskHandle_t receiverTaskHandle = nullptr;
TaskHandle_t enginesTaskHandle = nullptr;
TaskHandle_t failsafeTaskHandle = nullptr;

int main() {
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

    imuTaskHandle = xTaskCreateStatic(
        imuTask,
        "imuTask",
        256,
        NULL,
        5,
        imuTaskStack,
        &imuTaskBuffer);

    receiverTaskHandle = xTaskCreateStatic(
        receiverTask,
        "receiverTask",
        256,
        NULL,
        5,
        receiverTaskStack,
        &receiverTaskBuffer);

    enginesTaskHandle = xTaskCreateStatic(
        enginesTask,
        "enginesTask",
        256,
        NULL,
        5,
        enginesTaskStack,
        &enginesTaskBuffer);

    failsafeTaskHandle = xTaskCreateStatic(
        failsafeTask,
        "failsafeTask",
        256,
        NULL,
        5,
        failsafeTaskStack,
        &failsafeTaskBuffer);
}

void imuTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    while(1) {

    }
}

void receiverTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    std::array<uint8_t, DMA_SETUP::NDTR_VAL> rawPacket;
    uint32_t rawDataAddress{0};
    uint8_t* rawDataPtr{nullptr};

    while(1) {
        if (xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &rawDataAddress,
            pdMS_TO_TICKS(1000)
        ) == pdPASS) {
            rawDataPtr = reinterpret_cast<uint8_t*>(rawDataAddress);
            std::memcpy(rawPacket.data(), rawDataPtr, DMA_SETUP::NDTR_VAL);

            //rest of logic

            rawDataAddress = 0U;
        } else {
            xTaskNotifyGive(failsafeTaskHandle);
        }
    }
}

void enginesTask(void* pvParameters) {

    while(1) {
        
    }
}

void failsafeTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    //ensure that only failsafe task is able to operate engines
    vTaskDelete(enginesTaskHandle);
    vTaskDelete(receiverTaskHandle);

    while(1) {
        
    }
}
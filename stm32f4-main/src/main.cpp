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
#include "crc.hpp"

#include "dronePacket.hpp"
#include "control.hpp"

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

    void USART1_IRQHandler() {
        UsartHandle::instance->handleIRQ();
    }
} //NOLINTEND

void imuTask(void* pvParameters);
void receiverTask(void* pvParameters);
void enginesTask(void* pvParameters);
void failsafeTask(void* pvParameters);

StackType_t imuTaskStack[512];
StackType_t receiverTaskStack[512];
StackType_t enginesTaskStack[512];
StackType_t failsafeTaskStack[512];

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
    nvic.enableIRQ(NVIC_IRQs::USART1_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::USART1_IRQ, NVIC_IRQs::USART1_IRQ_Priority);

    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOAEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOBEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::DMA2EN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::CRCEN);

    rcc.enableAPB1PeripheralClock(APB1_PERIPHERAL_BITS::SPI2EN);

    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::USART1EN);
    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::TIM1EN);
    
    static GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    static GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);

    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH1);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH2);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH3);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH4);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH1);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH2);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH3);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH4);

    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::USART1_TX);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::USART1_RX);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF07, GPIOB_PINS::USART1_TX);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF07, GPIOB_PINS::USART1_RX);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::USART1_RX);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::USART1_TX);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::USART1_RX);

    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_MOSI);

    static SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);

    spi2.init(GPIOB_PINS::SPI2_SCS);

    static DmaHandle dma2(PeripheralBaseAddr::DMA2_BASEADDR);
    static UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);
    
    
    static Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);
    
    imuTaskHandle = xTaskCreateStatic
    (
        imuTask,
        "imuTask",
        512,
        NULL,
        3,
        imuTaskStack,
        &imuTaskBuffer
    );
    
    receiverTaskHandle = xTaskCreateStatic
    (
        receiverTask,
        "receiverTask",
        512,
        NULL,
        4,
        receiverTaskStack,
        &receiverTaskBuffer
    );
    
    enginesTaskHandle = xTaskCreateStatic
    (
        enginesTask,
        "enginesTask",
        512,
        NULL,
        2,
        enginesTaskStack,
        &enginesTaskBuffer
    );
    
    failsafeTaskHandle = xTaskCreateStatic
    (
        failsafeTask,
        "failsafeTask",
        512,
        NULL,
        5,
        failsafeTaskStack,
        &failsafeTaskBuffer
    );

    dma2.init(uart1.getDataRegAddr(), receiverTaskHandle);
    
    uart1.init();
    
    vTaskStartScheduler();

    while (1) {
    
    }

    return 0;;
}

void imuTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    while(1) {
        vTaskDelay(100);
    }
}

void receiverTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    uint32_t rawDataAddress{0};
    uint8_t* rawDataPtr{nullptr};
    
    uint32_t controlData;
    DroneControlPacket receivedPacket{};
    uint32_t crc{0};
    uint8_t badPacketsCounter{0};
    bool isInitialized{false};

    while(1) {
        if (xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &rawDataAddress,
            pdMS_TO_TICKS(1000)
        ) == pdPASS) {
            rawDataPtr = reinterpret_cast<uint8_t*>(rawDataAddress);
            std::memcpy(&receivedPacket, rawDataPtr, DMA_SETUP::NDTR_VAL);

            std::memcpy(&controlData, &receivedPacket, packetStructure::DATA_LENGTH);

            crc = calculateCRC(controlData);

            if (crc == receivedPacket.crcValue) {
                if (!isInitialized) {
                    isInitialized = true;
                }
                badPacketsCounter = 0;
                //update global control
            } else {
                badPacketsCounter++;
                if (badPacketsCounter > 5) {
                    xTaskNotifyGive(failsafeTaskHandle);
                }
            }

            crc = 0U;
            rawDataAddress = 0U;
        } else {
            if (isInitialized) {
                xTaskNotifyGive(failsafeTaskHandle);
            }
        }
    }
}

void enginesTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    while(1) {
        vTaskDelay(100);
    }
}

void failsafeTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    //ensure that only failsafe task is able to operate engines
    //vTaskDelete(enginesTaskHandle);
    //vTaskDelete(receiverTaskHandle);

    while(1) {
        vTaskDelay(100);
    }
}
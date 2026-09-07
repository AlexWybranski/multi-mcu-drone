#include <cstdint>
#include <array>

#include "FreeRTOSConfig.h"

#include "stm32f4.hpp"
#include "rcc.hpp"
#include "dma.hpp"
#include "tim.hpp"
#include "usart.hpp"
#include "spi.hpp"
#include "gpio.hpp"
#include "nvic.hpp"

extern "C" {
    void _init(void) {} 
}

int main() {
    // alignas(4) std::array<uint8_t, DMA_SETUP::NDTR_VAL> bufferOne{};
    // alignas(4) std::array<uint8_t, DMA_SETUP::NDTR_VAL> bufferTwo{};
    RccHandle rcc(PeripheralBaseAddr::RCC_BASEADDR);
    rcc.setClock();
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOAEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOBEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::DMA2EN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::CRCEN);

    rcc.enableAPB1PeripheralClock(APB1_PERIPHERAL_BITS::SPI2EN);

    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::USART1EN);
    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::TIM1EN);
    
    GpioHandle gpioA(PeripheralBaseAddr::GPIOA_BASEADDR);
    GpioHandle gpioB(PeripheralBaseAddr::GPIOB_BASEADDR);

    UsartHandle uart1(PeripheralBaseAddr::USART1_BASEADDR);

    SpiHandle spi2(PeripheralBaseAddr::SPI2_BASEADDR, &gpioB);

    DmaHandle dma2(PeripheralBaseAddr::DMA2_BASEADDR);

    Tim1Handle tim1(PeripheralBaseAddr::TIM1_BASEADDR);
}
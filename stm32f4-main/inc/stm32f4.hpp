#include <cstdint>

namespace PeripheralBaseAddr {
    constexpr uint32_t GPIOA_BASEADDR = 0x40020000U; 
    constexpr uint32_t GPIOB_BASEADDR = 0x40020400U; 
    constexpr uint32_t TIM1_BASEADDR = 0x40010000U;
    constexpr uint32_t RCC_BASEADDR = 0x40023800U;
    constexpr uint32_t SPI2_BASEADDR = 0x40013000U;
    constexpr uint32_t USART1_BASEADDR = 0x40011000U;
    constexpr uint32_t DMA2_BASEADDR = 0x40026400U;
}

namespace AHB1_PERIPHERAL_BITS {
    constexpr uint32_t DMA2EN = 22U;
    constexpr uint32_t CRCEN = 12U;
    constexpr uint32_t GPIOBEN = 1U;
    constexpr uint32_t GPIOAEN = 0U;
}

namespace APB1_PERIPHERAL_BITS {
    constexpr uint32_t SPI2EN = 14U;
}

namespace APB2_PERIPHERAL_BITS {
    constexpr uint32_t USART1EN = 4U;
    constexpr uint32_t TIM1EN = 0U;
}
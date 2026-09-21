#include <cstdint>

namespace PeripheralBaseAddr {
    constexpr uint32_t GPIOA_BASEADDR = 0x40020000U; 
    constexpr uint32_t GPIOB_BASEADDR = 0x40020400U; 
    constexpr uint32_t TIM1_BASEADDR = 0x40010000U;
    constexpr uint32_t RCC_BASEADDR = 0x40023800U;
    constexpr uint32_t SPI2_BASEADDR = 0x40003800U;
    constexpr uint32_t USART1_BASEADDR = 0x40011000U;
    constexpr uint32_t DMA2_BASEADDR = 0x40026400U;
    constexpr uint32_t CRC_BASEADDR = 0x40023000U;
    constexpr uint32_t NVIC_BASEADDR = 0xE000E000U;
}

namespace NVIC_IRQs {
    constexpr uint32_t DMA2_Stream5_IRQ = 68U;
    constexpr uint8_t DMA2_Stream5_IRQ_Priority = 6U;
    constexpr uint32_t SPI2_IRQ = 36U;
    constexpr uint8_t SPI2_IRQ_Priority = 7U;
    constexpr uint32_t USART1_IRQ = 37U;
    constexpr uint32_t USART1_IRQ_Priority = 5U;
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

namespace GPIOA_PINS {
    constexpr uint32_t T1_CH1 = 8U;
    constexpr uint32_t T1_CH2 = 9U;
    constexpr uint32_t T1_CH3 = 10U;
    constexpr uint32_t T1_CH4 = 11U;
}

namespace GPIOB_PINS {
    constexpr uint32_t IMU_INT1 = 5U;
    constexpr uint32_t USART1_TX = 6U;
    constexpr uint32_t USART1_RX = 7U;
    constexpr uint32_t SPI2_SCS = 8U;
    constexpr uint32_t SPI2_SCK = 13U;
    constexpr uint32_t SPI2_MISO = 14U;
    constexpr uint32_t SPI2_MOSI = 15U;
}
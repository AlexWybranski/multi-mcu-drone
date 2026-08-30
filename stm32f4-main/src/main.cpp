#include <cstdint>

extern "C" {
    void _init(void) {} 
}

namespace PeripheralBaseAddr {
    constexpr uint32_t GPIOA_BASEADDR = 0x40020000U; 
    constexpr uint32_t GPIOB_BASEADDR = 0x40020400U; 
    constexpr uint32_t TIM1_BASEADDR = 0x40010000U;
    constexpr uint32_t RCC_BASEADDR = 0x40023800U;
    constexpr uint32_t SPI2_BASEADDR = 0x40013000U;
    constexpr uint32_t USART2_BASEADDR = 0x40011000U;
}

int main() {
    
}
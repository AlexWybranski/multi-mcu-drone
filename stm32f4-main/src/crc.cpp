#include "crc.hpp"
#include "stm32f4.hpp"

extern "C" {
    #include "FreeRTOS.h" // IWYU pragma: keep
    #include "task.h"
}

uint32_t calculateCRC(uint32_t value) {
    // Hardware register mapping, NOLINT used
    auto* const l_CRC = reinterpret_cast<CRC_regs*>(PeripheralBaseAddr::CRC_BASEADDR); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    uint32_t calculatedCRC = 0U;

    l_CRC->DR = value;

    vTaskDelay(4);

    calculatedCRC = l_CRC->DR;

    l_CRC->CR = 1U;

    return calculatedCRC;
}
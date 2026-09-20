#include "crc.hpp"
#include "stm32f4.hpp"

extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}

uint32_t calculateCRC(uint32_t value) {
    auto* const l_CRC = reinterpret_cast<CRC_regs*>(PeripheralBaseAddr::CRC_BASEADDR);
    uint32_t calculatedCRC;

    l_CRC->DR = value;

    vTaskDelay(4);

    calculatedCRC = l_CRC->DR;

    l_CRC->CR = 1U;

    return calculatedCRC;
}
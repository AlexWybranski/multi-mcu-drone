#include "usart.hpp"
#include <cstdint>

/*
    This function ignores heavy math from F4 Reference manual, and only uses ready DIV_Mantissa and DIV_Fraction values

    Calculated values are for 115200 Baud rate with 48 MHz clock on APB1 bus
*/
constexpr uint32_t UsartHandle::calculateBRRregValue() {
    uint32_t val{0};
    uint32_t mantissa = 26U;
    uint32_t mantissa_reg = (mantissa << 4U);

    uint32_t fraction = 1U;
    uint32_t fraction_reg = (fraction << 0);

    val |= (mantissa_reg | fraction_reg);

    return val;
}
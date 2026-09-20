#ifndef CRC_HPP
#define CRC_HPP
#include <cstdint>

struct CRC_regs {
    volatile uint32_t DR;
    volatile uint32_t IDR;
    volatile uint32_t CR;
};

uint32_t calculateCRC(uint32_t value);

#endif
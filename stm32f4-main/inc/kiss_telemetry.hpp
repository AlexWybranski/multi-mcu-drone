#ifndef KISS_TELEMETRY_HPP
#define KISS_TELEMETRY_HPP

#include <cstdint>

struct __attribute__((packed)) KissTelemetry // sizeof(KissTelemetry) = 10
{
    int8_t temperature; // temp in *C
    uint8_t voltage_h; // voltage in uint16_t as voltage*100 in two bytes
    uint8_t voltage_l;
    uint8_t current_h;
    uint8_t current_l;
    uint8_t consumption_h;
    uint8_t consumption_l;
    uint8_t erpm_h;
    uint8_t erpm_l;
    uint8_t crc;
};

#endif
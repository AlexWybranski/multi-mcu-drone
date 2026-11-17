#include "dshot.hpp"

void DShot::assembleDshotBuffer(uint16_t duty, bool telemetry) {
    using namespace DSHOT;

    uint16_t dShotFrame{0};

    dShotFrame |= (duty << THROTTLE_SHIFT);
    
    if (telemetry) {
        dShotFrame |= (0b1U << TELEMETRY_SHIFT);
    }

    uint16_t crc = ((dShotFrame >> 4U) ^ (dShotFrame >> 8U) ^ (dShotFrame >> 12U)) & 0x0FU;

    dShotFrame |= (crc << CRC_SHIFT);

    for (uint32_t i{0}; i < 16U; ++i) {
        if(dShotFrame & (0b1U << (15U - i))) {
            dShotBuffer[i] = T1H;
        } else {
            dShotBuffer[i] = T0H;
        }
    }
}

void DShot::registerWritingFunc(DmaWritingFunc func) {
    writeFunction = func;
}

void DShot::registerStreamEnableFunc(DmaEnableStreamFunc func) {
    enableDmaStream = func;
}

void DShot::setupTransfer() {
    writeFunction(dShotBuffer.data(), DSHOT::DSHOT_BUFFER_SIZE);
}

void DShot::startTransfer() {
    enableDmaStream();
}
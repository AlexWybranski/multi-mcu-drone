#ifndef NVIC_HPP
#define NVIC_HPP

#include <cstdint>

namespace nvic_constants {
    constexpr uint32_t ISER_BASE_OFFSET = 0x100;
    constexpr uint32_t IPR_BASE_OFFSET = 0x400;
    constexpr uint32_t ISER_IPR_ADDITIONAL_OFFSET = 0x04;
    constexpr uint32_t ISER_IRQs_PER_REG = 32U;
    constexpr uint32_t IPR_IRQs_PER_REG = 4U;
    constexpr uint32_t IPR_BITS_PER_IRQ = 8U;

    constexpr uint32_t IRQ_MAX_NUMBER = 85U;
    constexpr uint8_t PRIORITY_MAX_NUMBER = 15U;

    constexpr uint32_t BIT_DIVIDE_BY_4 = 2U;
    constexpr uint32_t BIT_DIVIDE_BY_32 = 5U;
}

class Nvic {
    private:
        uint32_t m_NVIC;

    public:
        explicit Nvic(uint32_t baseAddr) : m_NVIC(baseAddr) {}
        ~Nvic() = default;
        Nvic(const Nvic& other) = delete;
        Nvic& operator=(const Nvic& other) = delete;
        Nvic(Nvic&& other) = delete;
        Nvic& operator=(Nvic&& other) = delete;

        void enableIRQ(uint32_t irqNum);

        void setIrqPriority(uint32_t irqNum, uint8_t priority);
};

#endif
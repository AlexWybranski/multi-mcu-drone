#include "nvic.hpp"

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

/*
    To write into proper ISER register it's important to derive proper x, where:
    for ISER x = 0...7, rule is:
    - One ISER reg holds 32 bits for 32 interrupts
    
    On STMF411 there are 85 interrupts
*/
void Nvic::enableIRQ(uint32_t irqNum) {
    using namespace nvic_constants;

    if(irqNum > IRQ_MAX_NUMBER) {
        return;
    }    
            
    volatile uint32_t* ISERx = nullptr;
            
    auto offsetMultiplier = static_cast<uint8_t>(irqNum >> BIT_DIVIDE_BY_32);
                
    uint32_t offset = (ISER_BASE_OFFSET + (ISER_IPR_ADDITIONAL_OFFSET * offsetMultiplier));
    
    //NOLINT used, because of mapping hardware register
    ISERx = reinterpret_cast<volatile uint32_t*>(m_NVIC + offset); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
                
    const uint32_t shift = irqNum % ISER_IRQs_PER_REG;
                
    *ISERx |= (0b1U << shift);
}

/*
    To write into proper IPR register it's important to derive proper x, where:
    for IPR x = 0..59, rule is:
    - One IPR reg holds 32 bits for 4 interrupts (8 per interrupt) BUT: STM supports only 4 bits 
    
    On STMF411 there are 85 interrupts
*/
void Nvic::setIrqPriority(uint32_t irqNum, uint8_t priority) {
    using namespace nvic_constants;

    if(priority > PRIORITY_MAX_NUMBER || irqNum > IRQ_MAX_NUMBER) {
        return;
    }

    volatile uint32_t* IPRx = nullptr;

    const uint8_t resetValue = 0b11111111U;

    auto offsetMultiplier = static_cast<uint8_t>(irqNum >> BIT_DIVIDE_BY_4);

    uint32_t offset = (IPR_BASE_OFFSET + (ISER_IPR_ADDITIONAL_OFFSET * offsetMultiplier));

    //NOLINT used, because of mapping hardware register
    IPRx = reinterpret_cast<volatile uint32_t*>(m_NVIC + offset); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)

    uint32_t shift = (irqNum % IPR_IRQs_PER_REG) * IPR_BITS_PER_IRQ;

    *IPRx &= ~((static_cast<uint32_t>(resetValue)) << shift); 
    *IPRx |= ((static_cast<uint32_t>(priority)) << (shift + 4));
}
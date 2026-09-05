#include "rcc.hpp"
#include <cstdint>

namespace RCC_SETUP {
    constexpr uint32_t FLASH_BASEADDR = 0x40023C00;
    constexpr uint32_t FLASH_ACR_LATENCY_SET = (0b0001U << 0U);
    constexpr uint32_t FLASH_ACR_LATENCY_RESET = ~(0b1111U << 0U);

    constexpr uint32_t CFGR_SW_RESET_VAL = 0b11U;
    constexpr uint32_t CFGR_SW_VAL = 0b10U;
    constexpr uint32_t CFGR_SW_SHIFT = 0U;
    constexpr uint32_t CFGR_SWS_DESIRED_VAL = 0b10U;
    constexpr uint32_t CFGR_SWS_SHIFT = 2U;
    constexpr uint32_t CFGR_PPRE1_RESET_VAL = 0b111U;
    constexpr uint32_t CFGR_PPRE1_SHIFT = 10U;
    constexpr uint32_t CFGR_PPRE2_RESET_VAL = 0b111U;
    constexpr uint32_t CFGR_PPRE2_SHIFT = 13U;

    constexpr uint32_t HSEON_VAL = 0b1U;
    constexpr uint32_t HSERDY_VAL = 0b1U;
    constexpr uint32_t HSEON_SHIFT = 16U;
    constexpr uint32_t HSERDY_SHIFT = 17U;

    constexpr uint32_t PLLON_VAL = 0b1U;  
    constexpr uint32_t PLLRDY_VAL = 0b1U;  
    constexpr uint32_t PLLON_SHIFT = 24U;
    constexpr uint32_t PLLRDY_SHIFT = 25U;
}

constexpr uint32_t RccHandle::calculatePllCfgrValue() {
    const uint32_t PLLSRC_SHIFT = 22U;
    const uint32_t PLLM_SHIFT = 0;
    const uint32_t PLLN_SHIFT = 6U;
    const uint32_t PLLP_SHIFT = 16U;
    const uint32_t PLLQ_SHIFT = 24U;

    //Reset value for RCC_PLLCFGR register
    const uint32_t resetVal = 0x24003010U;

    const uint32_t PLL_SRC_RESET = 1U;
    const uint32_t M_VAL_RESET = 63U;
    const uint32_t N_VAL_RESET = 511U;
    const uint32_t P_VAL_RESET = 0b11U;
    const uint32_t Q_VAL_RESET = 15U;

    const uint32_t PLL_SRC = 1U; //HSE will be source for PLL
    /*
        HSE base frequency is 25MHz
        PLL VCO Clock = Clock input * (N / M) or Clock input / M * N
        PLL General Clock Output = VCO Clock / P
        USB OTG FS, SDIO = VCO Clock / Q
    */
    const uint32_t M_VAL = 25U;
    const uint32_t N_VAL = 192U;
    const uint32_t P_VAL = 0b01U;
    const uint32_t Q_VAL = 4U;

    uint32_t val = resetVal;

    val &= ~((PLL_SRC_RESET << PLLSRC_SHIFT) | (M_VAL_RESET << PLLM_SHIFT) | (N_VAL_RESET << PLLN_SHIFT) | (P_VAL_RESET << PLLP_SHIFT) | (Q_VAL_RESET << PLLQ_SHIFT));
    val |= ((PLL_SRC << PLLSRC_SHIFT) | (M_VAL << PLLM_SHIFT) | (N_VAL << PLLN_SHIFT) | (P_VAL << PLLP_SHIFT) | (Q_VAL << PLLQ_SHIFT));

    return val;
} 

void RccHandle::setClock() {
    using namespace RCC_SETUP;
    /*
        FLASH_ACR register which is needed is first register in flash peripheral -> no struct needed, writing straight to base address
    */
    auto* const l_FLASH = reinterpret_cast<volatile uint32_t*>(FLASH_BASEADDR); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    
    m_RCC->CFGR &= ~(CFGR_PPRE1_RESET_VAL << CFGR_PPRE1_SHIFT);
    m_RCC->CFGR &= ~(CFGR_PPRE2_RESET_VAL << CFGR_PPRE2_SHIFT);
    m_RCC->PLLCFGR = calculatePllCfgrValue();

    *l_FLASH &= FLASH_ACR_LATENCY_RESET;
    *l_FLASH |= FLASH_ACR_LATENCY_SET;

    m_RCC->CR |= (HSEON_VAL << HSEON_SHIFT);
    while ((m_RCC->CR & (HSERDY_VAL << HSERDY_SHIFT)) == 0) {}

    m_RCC->CR |= (PLLON_VAL << PLLON_SHIFT);
    while ((m_RCC->CR & (PLLRDY_VAL << PLLRDY_SHIFT)) == 0) {}

    m_RCC->CFGR &= ~(CFGR_SW_RESET_VAL << CFGR_SW_SHIFT);
    m_RCC->CFGR |= (CFGR_SW_VAL << CFGR_SW_SHIFT);

    while ((m_RCC->CFGR & (CFGR_SWS_DESIRED_VAL << CFGR_SWS_SHIFT)) == 0) {}
}

void RccHandle::enableAHB1PeripheralClock(uint32_t peripheralBit) {
    uint32_t PERIPHERAL_EN = (0b1U << peripheralBit);

    m_RCC->AHB1ENR |= PERIPHERAL_EN;
}

void RccHandle::enableAPB1PeripheralClock(uint32_t peripheralBit) {
    uint32_t PERIPHERAL_EN = (0b1U << peripheralBit);

    m_RCC->APB1ENR |= PERIPHERAL_EN;
}

void RccHandle::enableTim1Clock() {
    constexpr uint32_t TIM1_EN = (0b1U << 0U);

    m_RCC->APB2ENR |= TIM1_EN;
}
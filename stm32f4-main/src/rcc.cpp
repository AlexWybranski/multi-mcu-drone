#include "rcc.hpp"

consteval uint32_t RccHandle::calculatePllCfgrValue() {
    const uint32_t PLLSRC_SHIFT = 22U;
    const uint32_t PLLM_SHIFT = 0;
    const uint32_t PLLN_SHIFT = 6U;
    const uint32_t PLLP_SHIFT = 16U;
    const uint32_t PLLQ_SHIFT = 24U;

    //Reset value for RCC_PLLCFGR register
    const uint32_t resetVal = 0x24003010U;

    const uint32_t PLL_SRC_RESET = (1U << PLLSRC_SHIFT);
    const uint32_t M_VAL_RESET = (63U << PLLM_SHIFT);
    const uint32_t N_VAL_RESET = (511U << PLLN_SHIFT);
    const uint32_t P_VAL_RESET = (0b11U << PLLP_SHIFT);
    const uint32_t Q_VAL_RESET = (15U << PLLQ_SHIFT);

    const uint32_t PLL_SRC = (1U << PLLSRC_SHIFT); //HSE will be source for PLL
    /*
        HSE base frequency is 25MHz
        PLL VCO Clock = Clock input * (N / M) or Clock input / M * N
        PLL General Clock Output = VCO Clock / P
        USB OTG FS, SDIO = VCO Clock / Q
    */
    const uint32_t M_VAL = (25U << PLLM_SHIFT);
    const uint32_t N_VAL = (192U << PLLN_SHIFT);
    const uint32_t P_VAL = (0b01U << PLLP_SHIFT);
    const uint32_t Q_VAL = (4U << PLLQ_SHIFT);

    uint32_t val = resetVal;

    val &= ~(PLL_SRC_RESET | M_VAL_RESET | N_VAL_RESET | P_VAL_RESET | Q_VAL_RESET);
    val |= (PLL_SRC | M_VAL | N_VAL | P_VAL | Q_VAL);

    return val;
} 

void RccHandle::setClock() {
    using namespace RCC_SETUP;
    /*
        FLASH_ACR register which is needed is first register in flash peripheral -> no struct needed, writing straight to base address
    */
    auto* const l_FLASH = reinterpret_cast<volatile uint32_t*>(FLASH_BASEADDR); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    
    m_RCC->CFGR &= ~CFGR_PPRE1_RESET;
    m_RCC->CFGR &= ~CFGR_PPRE2_RESET;
    m_RCC->PLLCFGR = calculatePllCfgrValue();

    *l_FLASH &= FLASH_ACR_LATENCY_RESET;
    *l_FLASH |= FLASH_ACR_LATENCY_SET;

    m_RCC->CR |= CR_HSEON;
    while ((m_RCC->CR & CR_HSERDY) == 0) {}

    m_RCC->CR |= CR_PLLON;
    while ((m_RCC->CR & CR_PLLRDY) == 0) {}

    m_RCC->CFGR &= ~CFGR_SW_RESET;
    m_RCC->CFGR |= CFGR_SW_SET;

    while ((m_RCC->CFGR & CFGR_SWS_DESIRED_VAL) == 0) {}
}

void RccHandle::enableAHB1PeripheralClock(uint32_t peripheralBit) {
    uint32_t PERIPHERAL_EN = (0b1U << peripheralBit);

    if(m_RCC->AHB1ENR & PERIPHERAL_EN) {
        return;
    }

    m_RCC->AHB1ENR |= PERIPHERAL_EN;
}

void RccHandle::enableAPB1PeripheralClock(uint32_t peripheralBit) {
    uint32_t PERIPHERAL_EN = (0b1U << peripheralBit);

    if(m_RCC->APB1ENR & PERIPHERAL_EN) {
        return;
    }

    m_RCC->APB1ENR |= PERIPHERAL_EN;
}

void RccHandle::enableAPB2PeripheralClock(uint32_t peripheralBit) {
    uint32_t PERIPHERAL_EN = (0b1U << peripheralBit);

    if(m_RCC->APB2ENR & PERIPHERAL_EN) {
        return;
    }

    m_RCC->APB2ENR |= PERIPHERAL_EN;
}
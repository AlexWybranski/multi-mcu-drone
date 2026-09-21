#include <stdint.h>

#define SRAM_START      0x20000000U
#define SRAM_SIZE       (128 * 1024)
#define SRAM_END        ((SRAM_START)+(SRAM_SIZE))

#define _ESTACK         SRAM_END

extern uint32_t __text_end__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t _sidata;

int main();
void __libc_init_array(void);
void Default_Handler(void) {
    while(1) {}
}

//handlers prototypes
void Reset_Handler(void);
void NMI_Handler(void)                      __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)                __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)                __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)                 __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)               __attribute__((weak, alias("Default_Handler")));
void SVCall_Handler(void)                   __attribute__((weak, alias("Default_Handler")));
void Debug_Monitor_Handler(void)            __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)                   __attribute__((weak, alias("Default_Handler")));
void Systick_Handler(void)                  __attribute__((weak, alias("Default_Handler")));
void WWDG_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)                   __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)                   __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler(void)                   __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI17_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void EXTI18_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void)                   __attribute__((weak, alias("Default_Handler")));
void SPI4_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));
void SPI5_IRQHandler(void)                  __attribute__((weak, alias("Default_Handler")));

//vector table
uint32_t vector_table[] __attribute__((section (".isr_vector"))) = {
    _ESTACK,
    (uint32_t)(uintptr_t)&Reset_Handler,
    (uint32_t)(uintptr_t)&NMI_Handler,
    (uint32_t)(uintptr_t)&HardFault_Handler,
    (uint32_t)(uintptr_t)&MemManage_Handler,
    (uint32_t)(uintptr_t)&BusFault_Handler,
    (uint32_t)(uintptr_t)&UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&SVCall_Handler,
    (uint32_t)(uintptr_t)&Debug_Monitor_Handler,
    0,
    (uint32_t)(uintptr_t)&PendSV_Handler,
    (uint32_t)(uintptr_t)&Systick_Handler,
    (uint32_t)(uintptr_t)&WWDG_IRQHandler,
    (uint32_t)(uintptr_t)&PVD_IRQHandler,
    (uint32_t)(uintptr_t)&TAMP_STAMP_IRQHandler,
    (uint32_t)(uintptr_t)&RTC_WKUP_IRQHandler,
    (uint32_t)(uintptr_t)&FLASH_IRQHandler,
    (uint32_t)(uintptr_t)&RCC_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI0_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI1_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI2_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI3_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI4_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream0_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream1_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream2_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream3_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream4_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream5_IRQHandler,
    (uint32_t)(uintptr_t)&DMA1_Stream6_IRQHandler,
    (uint32_t)(uintptr_t)&ADC_IRQHandler,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&EXTI9_5_IRQHandler,
    (uint32_t)(uintptr_t)&TIM1_BRK_TIM9_IRQHandler,
    (uint32_t)(uintptr_t)&TIM1_UP_TIM10_IRQHandler,
    (uint32_t)(uintptr_t)&TIM1_TRG_COM_TIM11_IRQHandler,
    (uint32_t)(uintptr_t)&TIM1_CC_IRQHandler,
    (uint32_t)(uintptr_t)&TIM2_IRQHandler,
    (uint32_t)(uintptr_t)&TIM3_IRQHandler,
    (uint32_t)(uintptr_t)&TIM4_IRQHandler,
    (uint32_t)(uintptr_t)&I2C1_EV_IRQHandler,
    (uint32_t)(uintptr_t)&I2C1_ER_IRQHandler,
    (uint32_t)(uintptr_t)&I2C2_EV_IRQHandler,
    (uint32_t)(uintptr_t)&I2C2_ER_IRQHandler,
    (uint32_t)(uintptr_t)&SPI1_IRQHandler,
    (uint32_t)(uintptr_t)&SPI2_IRQHandler,
    (uint32_t)(uintptr_t)&USART1_IRQHandler,
    (uint32_t)(uintptr_t)&USART2_IRQHandler,
    0,
    (uint32_t)(uintptr_t)&EXTI15_10_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI17_IRQHandler,
    (uint32_t)(uintptr_t)&EXTI18_IRQHandler,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&DMA1_Stream7_IRQHandler,
    0,
    (uint32_t)(uintptr_t)&SDIO_IRQHandler,
    (uint32_t)(uintptr_t)&TIM5_IRQHandler,
    (uint32_t)(uintptr_t)&SPI3_IRQHandler,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&DMA2_Stream0_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream1_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream2_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream3_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream4_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&OTG_FS_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream5_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream6_IRQHandler,
    (uint32_t)(uintptr_t)&DMA2_Stream7_IRQHandler,
    (uint32_t)(uintptr_t)&USART6_IRQHandler,
    (uint32_t)(uintptr_t)&I2C3_EV_IRQHandler,
    (uint32_t)(uintptr_t)&I2C3_ER_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t)(uintptr_t)&FPU_IRQHandler,
    0,
    0,
    (uint32_t)(uintptr_t)&SPI4_IRQHandler,
    (uint32_t)(uintptr_t)&SPI5_IRQHandler
};

void Reset_Handler(void) {
    uint32_t size = (uint32_t)&__data_end__ - (uint32_t)&__data_start__;

    uint8_t *pDest = (uint8_t*)&__data_start__;
    uint8_t *pSrc = (uint8_t*)&_sidata;

    for(uint32_t i = 0; i < size; ++i) {
        *pDest++ = *pSrc++;
    }

    uint32_t bss_size = (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__;
    pDest = (uint8_t*)&__bss_start__;

    for(uint32_t i = 0; i < bss_size; ++i) {
        *pDest++ = 0;
    }

    __libc_init_array();
    main();
}
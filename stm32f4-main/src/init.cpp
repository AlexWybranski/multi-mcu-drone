#include "init.hpp"

void initClocks(RccHandle& rcc) {
    rcc.setClock();

    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOAEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::GPIOBEN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::DMA2EN);
    rcc.enableAHB1PeripheralClock(AHB1_PERIPHERAL_BITS::CRCEN);

    rcc.enableAPB1PeripheralClock(APB1_PERIPHERAL_BITS::SPI2EN);

    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::USART1EN);
    rcc.enableAPB2PeripheralClock(APB2_PERIPHERAL_BITS::TIM1EN);
}

void initIRQs(Nvic& nvic) {
    nvic.enableIRQ(NVIC_IRQs::SPI2_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::SPI2_IRQ, NVIC_IRQs::SPI2_IRQ_Priority);
    nvic.enableIRQ(NVIC_IRQs::DMA2_Stream5_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::DMA2_Stream5_IRQ, NVIC_IRQs::DMA2_Stream5_IRQ_Priority);
    nvic.enableIRQ(NVIC_IRQs::USART1_IRQ);
    nvic.setIrqPriority(NVIC_IRQs::USART1_IRQ, NVIC_IRQs::USART1_IRQ_Priority);
}

void initGpio(GpioHandle& gpioA, GpioHandle& gpioB) {
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH1);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH2);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH3);
    gpioA.setPinMode(GpioHandle::Mode::alternateFunction, GPIOA_PINS::T1_CH4);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH1);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH2);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH3);
    gpioA.setPinAlternateFunction(GpioHandle::Func::AF01, GPIOA_PINS::T1_CH4);

    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::USART1_TX);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::USART1_RX);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF07, GPIOB_PINS::USART1_TX);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF07, GPIOB_PINS::USART1_RX);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::USART1_RX);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::USART1_TX);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::USART1_RX);

    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinMode(GpioHandle::Mode::alternateFunction, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinOutputSpeed(GpioHandle::Speed::medium, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinAlternateFunction(GpioHandle::Func::AF05, GPIOB_PINS::SPI2_MOSI);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_SCK);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_MISO);
    gpioB.setPinPullupPulldown(GpioHandle::Pull::pullup, GPIOB_PINS::SPI2_MOSI);
}
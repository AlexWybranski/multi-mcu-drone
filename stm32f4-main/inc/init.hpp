#ifndef INIT_HPP
#define INIT_HPP

#include "stm32f4.hpp" // IWYU pragma: keep
#include "rcc.hpp"
#include "gpio.hpp"
#include "nvic.hpp"

void initClocks(RccHandle& rcc);

void initIRQs(Nvic& nvic);

void initGpio(GpioHandle& gpioA, GpioHandle& gpioB);

#endif
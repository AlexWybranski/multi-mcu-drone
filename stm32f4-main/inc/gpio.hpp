#ifndef GPIO_HPP
#define GPIO_HPP
#include <cstdint>

struct GPIO_regs {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
};

class GpioHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        GPIO_regs* const m_GPIO; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit GpioHandle(uint32_t baseAddr) : m_GPIO(reinterpret_cast<GPIO_regs*>(baseAddr)) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        ~GpioHandle() = default;
        GpioHandle(const GpioHandle& other) = delete;
        GpioHandle& operator=(const GpioHandle& other) = delete;
        GpioHandle(GpioHandle&& other) = delete;
        GpioHandle& operator=(GpioHandle&& other) = delete;

        enum class Mode : uint32_t {
            input               = 0b00U,
            output              = 0b01U,
            alternateFunction   = 0b10U,
            analog              = 0b11U
        };

        enum class Speed : uint32_t {
            low                 = 0b00U,
            medium              = 0b01U,
            fast                = 0b10U,
            high                = 0b11U
        };

        enum class Pull : uint32_t {
            none                = 0b00U,
            pullup              = 0b01U,
            pulldown            = 0b10U,
            reserved            = 0b11U
        };

        /*
            Since alternate functions varies between pins and ports always verify correct function for purpose in datasheet
        */
        enum class Func : uint32_t {
            AF00 = 0b0000U,
            AF01 = 0b0001U,
            AF02 = 0b0010U,
            AF03 = 0b0011U,
            AF04 = 0b0100U,
            AF05 = 0b0101U,
            AF06 = 0b0110U,
            AF07 = 0b0111U,
            AF08 = 0b1000U,
            AF09 = 0b1001U,
            AF10 = 0b1010U,
            AF11 = 0b1011U,
            AF12 = 0b1100U,
            AF13 = 0b1101U,
            AF14 = 0b1110U,
            AF15 = 0b1111U,
        };

        void setPinMode(Mode mode, uint32_t pinNum);

        void setPinOutputType(bool isOpenDrain, uint32_t pinNum);

        void setPinOutputSpeed(Speed speed, uint32_t pinNum);

        void setPinPullupPulldown(Pull pull, uint32_t pinNum);

        void setPinState(bool state, uint32_t pinNum);

        void setPinAlternateFunction(Func func, uint32_t pinNum);
};

#endif

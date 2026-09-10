#ifndef DMA_HPP
#define DMA_HPP
#include <cstdint>

namespace DMA_SETUP {
    //SxCR
    constexpr uint32_t CHANNEL_RESET = ~(0b111U << 25U);
    constexpr uint32_t CHANNEL4_SET = (0b100U << 25U);

    constexpr uint32_t DOUBLE_BUFFER_MODE = (0b1U << 19U);

    constexpr uint32_t MSIZE_ONE_BYTE = ~(0b11U << 13U);
    constexpr uint32_t PSIZE_ONE_BYTE = ~(0b11U << 11U);

    constexpr uint32_t MINC_ON = (0b1U << 10U);
    constexpr uint32_t PINC_OFF = ~(0b1U << 9U);

    constexpr uint32_t PER_TO_MEM_DIRECTION = ~(0b11U << 6U);

    constexpr uint32_t TCIE = (0b1U << 4U);

    constexpr uint32_t ENABLE = (0b1U << 0U);

    //value for SxNDTR register (what is length of one transfer (according to MSIZE value))
    constexpr uint32_t NDTR_VAL = 8U;

    //HIFCR
    constexpr uint32_t CLEAR_TCF = (0b1U << 11U); //CTCIFx bit
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct DMA_regs {
    volatile uint32_t LISR;
    volatile uint32_t HISR;
    volatile uint32_t LIFCR;
    volatile uint32_t HIFCR;
    volatile uint32_t S0CR;
    volatile uint32_t S0NDTR;
    volatile uint32_t S0PAR;
    volatile uint32_t S0M0AR;
    volatile uint32_t S0M1AR;
    volatile uint32_t S0FCR;
    volatile uint32_t S1CR;
    volatile uint32_t S1NDTR;
    volatile uint32_t S1PAR;
    volatile uint32_t S1M0AR;
    volatile uint32_t S1M1AR;
    volatile uint32_t S1FCR;
    volatile uint32_t S2CR;
    volatile uint32_t S2NDTR;
    volatile uint32_t S2PAR;
    volatile uint32_t S2M0AR;
    volatile uint32_t S2M1AR;
    volatile uint32_t S2FCR;
    volatile uint32_t S3CR;
    volatile uint32_t S3NDTR;
    volatile uint32_t S3PAR;
    volatile uint32_t S3M0AR;
    volatile uint32_t S3M1AR;
    volatile uint32_t S3FCR;
    volatile uint32_t S4CR;
    volatile uint32_t S4NDTR;
    volatile uint32_t S4PAR;
    volatile uint32_t S4M0AR;
    volatile uint32_t S4M1AR;
    volatile uint32_t S4FCR;
    volatile uint32_t S5CR;
    volatile uint32_t S5NDTR;
    volatile uint32_t S5PAR;
    volatile uint32_t S5M0AR;
    volatile uint32_t S5M1AR;
    volatile uint32_t S5FCR;
    volatile uint32_t S6CR;
    volatile uint32_t S6NDTR;
    volatile uint32_t S6PAR;
    volatile uint32_t S6M0AR;
    volatile uint32_t S6M1AR;
    volatile uint32_t S6FCR;
    volatile uint32_t S7CR;
    volatile uint32_t S7NDTR;
    volatile uint32_t S7PAR;
    volatile uint32_t S7M0AR;
    volatile uint32_t S7M1AR;
    volatile uint32_t S7FCR;
};

class DmaHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        DMA_regs* const m_DMA; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit DmaHandle(uint32_t baseAddr) : m_DMA(reinterpret_cast<DMA_regs*>(baseAddr)) { // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            instance = this;
        } 
        ~DmaHandle() = default;
        DmaHandle(const DmaHandle& other) = delete;
        DmaHandle& operator=(const DmaHandle& other) = delete;
        DmaHandle(DmaHandle&& other) = delete;
        DmaHandle& operator=(DmaHandle&& other) = delete;

        static DmaHandle* instance;

        /*
            This function initializes DMA to work on:
            - Stream 5 - Channel 4 -> USART1 with DMA2
            - Direct Mode
            - Double buffer
            - 1 byte word
        */
        void init(uint32_t* peripheral_reg_addr, uint8_t* bufferOne, uint8_t* bufferTwo);

        void handleIRQ();
};

#endif
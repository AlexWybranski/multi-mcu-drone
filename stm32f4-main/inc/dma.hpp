#ifndef DMA_HPP
#define DMA_HPP
#include <cstdint>
#include <array>

#include "dronePacket.hpp"

extern "C" {
    #include "FreeRTOS.h" // IWYU pragma: keep
    #include "task.h"
}

namespace DMA_SETUP {
    //Streams
    constexpr uint32_t STREAM_0 = 0U;
    constexpr uint32_t STREAM_1 = 1U;
    constexpr uint32_t STREAM_2 = 2U;
    constexpr uint32_t STREAM_3 = 3U;
    constexpr uint32_t STREAM_4 = 4U;
    constexpr uint32_t STREAM_5 = 5U;
    constexpr uint32_t STREAM_6 = 6U;
    constexpr uint32_t STREAM_7 = 7U;

    //SxCR
    constexpr uint32_t CHANNEL_RESET = ~(0b111U << 25U);
    constexpr uint32_t CHANNEL0_SET = ~(0b111U << 25U);
    constexpr uint32_t CHANNEL1_SET = (0b001U << 25U);
    constexpr uint32_t CHANNEL2_SET = (0b010U << 25U);
    constexpr uint32_t CHANNEL3_SET = (0b011U << 25U);
    constexpr uint32_t CHANNEL4_SET = (0b100U << 25U);
    constexpr uint32_t CHANNEL5_SET = (0b101U << 25U);
    constexpr uint32_t CHANNEL6_SET = (0b110U << 25U);
    constexpr uint32_t CHANNEL7_SET = (0b111U << 25U);

    constexpr uint32_t PRIORITY_LOW = ~(0b11U << 16U);
    constexpr uint32_t PRIORITY_MEDIUM = (0b01U << 16U);
    constexpr uint32_t PRIORITY_HIGH = (0b10U << 16U);
    constexpr uint32_t PRIORITY_VERY_HIGH = (0b11U << 16U);

    constexpr uint32_t DOUBLE_BUFFER_MODE = (0b1U << 18U);

    constexpr uint32_t MSIZE_ONE_BYTE = ~(0b11U << 13U);
    constexpr uint32_t PSIZE_ONE_BYTE = ~(0b11U << 11U);

    constexpr uint32_t MINC_ON = (0b1U << 10U);
    constexpr uint32_t MINC_OFF = ~(0b1U << 10U);
    constexpr uint32_t PINC_ON = (0b1U << 9U);
    constexpr uint32_t PINC_OFF = ~(0b1U << 9U);

    constexpr uint32_t PER_TO_MEM_DIRECTION = ~(0b11U << 6U);
    constexpr uint32_t MEM_TO_PER_DIRECTION = (0b01U << 6U);

    constexpr uint32_t CURRENT_TARGET = (0b1U << 19U);

    constexpr uint32_t TCIE = (0b1U << 4U);

    constexpr uint32_t ENABLE = (0b1U << 0U);

    //value for SxNDTR register (what is length of one transfer (according to MSIZE value))
    constexpr uint32_t UART_NDTR_VAL = sizeof(DroneControlPacket);
    constexpr uint32_t SPI_NDTR_VAL = 13U; //TEMP

    //HIFCR and LIFCR
    static constexpr std::array<uint32_t, 8> CLEAR_TCF {
        (0b1U << 5U),
        (0b1U << 11U),
        (0b1U << 21U),
        (0b1U << 27U),
        (0b1U << 5U),
        (0b1U << 11U),
        (0b1U << 21U),
        (0b1U << 27U)
    };

    //HISR and LISR
    static constexpr std::array<uint32_t, 8> TCIF {
        (0b1U << 5U),
        (0b1U << 11U),
        (0b1U << 21U),
        (0b1U << 27U),
        (0b1U << 5U),
        (0b1U << 11U),
        (0b1U << 21U),
        (0b1U << 27U)
    };
}

enum class DmaDirection : uint32_t {
    per_to_mem = DMA_SETUP::PER_TO_MEM_DIRECTION,
    mem_to_per = DMA_SETUP::MEM_TO_PER_DIRECTION,
};

enum class DmaChannel : uint32_t {
    ch0 = DMA_SETUP::CHANNEL0_SET,
    ch1 = DMA_SETUP::CHANNEL1_SET,
    ch2 = DMA_SETUP::CHANNEL2_SET,
    ch3 = DMA_SETUP::CHANNEL3_SET,
    ch4 = DMA_SETUP::CHANNEL4_SET,
    ch5 = DMA_SETUP::CHANNEL5_SET,
    ch6 = DMA_SETUP::CHANNEL6_SET,
    ch7 = DMA_SETUP::CHANNEL7_SET,
};

enum class DmaPriority : uint32_t {
    low = DMA_SETUP::PRIORITY_LOW,
    medium = DMA_SETUP::PRIORITY_MEDIUM,
    high = DMA_SETUP::PRIORITY_HIGH,
    very_high = DMA_SETUP::PRIORITY_VERY_HIGH,
};

enum class DmaMode : uint32_t {
    single = 0,
    double_buffer = 1,
};

struct DmaStreamConfig {
    DmaChannel channel;
    DmaDirection direction;
    DmaPriority priority;
    DmaMode mode;
};

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct DMA_Stream_regs {
    volatile uint32_t SCR;
    volatile uint32_t SNDTR;
    volatile uint32_t SPAR;
    volatile uint32_t SM0AR;
    volatile uint32_t SM1AR;
    volatile uint32_t SFCR;
};


// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct DMA_regs {
    volatile uint32_t LISR;
    volatile uint32_t HISR;
    volatile uint32_t LIFCR;
    volatile uint32_t HIFCR;
    DMA_Stream_regs StreamRegs[8];
};

template<uint32_t dmaBaseAddr, uint32_t streamNum, DmaStreamConfig config, size_t NDTR_VAL>
class DmaStreamHandle {
    private:
        //NOLINT used to ensure the peripheral's base address pointer remain constant
        DMA_regs* const m_DMA; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        DMA_Stream_regs* const m_DMA_Stream;

        std::array<volatile uint8_t, NDTR_VAL> bufferOne;
        std::array<volatile uint8_t, NDTR_VAL> bufferTwo;

        TaskHandle_t m_taskToNotify{};
    public:
        //reinterpret_cast is needed to map hardware register to code, NOLINT used
        explicit DmaStreamHandle() : m_DMA(reinterpret_cast<DMA_regs*>(dmaBaseAddr)), m_DMA_Stream(&(reinterpret_cast<DMA_regs*>(dmaBaseAddr))->StreamRegs[streamNum]) {} // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        
        ~DmaStreamHandle() = default;
        DmaStreamHandle(const DmaStreamHandle& other) = delete;
        DmaStreamHandle& operator=(const DmaStreamHandle& other) = delete;
        DmaStreamHandle(DmaStreamHandle&& other) = delete;
        DmaStreamHandle& operator=(DmaStreamHandle&& other) = delete;

        static DmaStreamHandle* instance;

        // In this driver reinterpret_cast is needed to change address to uint32_t number in order to write value to register or to send it via FreeRTOS Notification register
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        /*
            @param peripheral_reg_addr - register from/to which dma will read/write data
            @param taskToNotify - [optional] task that will be notified and get pointer to buffer to read data from
        */
        void init(volatile uint32_t* peripheral_reg_addr, TaskHandle_t taskToNotify) {
            using namespace DMA_SETUP;

            if(taskToNotify != nullptr) {
                m_taskToNotify = taskToNotify;
            }

            m_DMA_Stream->SCR &= ~ENABLE;

            while (m_DMA_Stream->SCR & ENABLE) {}
            
            if constexpr (streamNum >= 4) {
                m_DMA->HIFCR = CLEAR_TCF[streamNum];
            } else {
                m_DMA->LIFCR = CLEAR_TCF[streamNum];
            }
            
            uint32_t SxCR_MASK = 0U; 

            SxCR_MASK |= (
                static_cast<uint32_t>(config.channel) |
                static_cast<uint32_t>(config.direction) |
                static_cast<uint32_t>(config.priority) |
                //constant bits to all used configurations
                PINC_OFF |
                MINC_ON |
                MSIZE_ONE_BYTE |
                PSIZE_ONE_BYTE |
                TCIE
            );

            if constexpr (config.mode == DmaMode::double_buffer) {
                SxCR_MASK |= DOUBLE_BUFFER_MODE;
            }

            m_DMA_Stream->SCR = SxCR_MASK;
            
            m_DMA_Stream->SNDTR = NDTR_VAL;
            
            m_DMA_Stream->SPAR = std::bit_cast<uint32_t>(peripheral_reg_addr);
            
            m_DMA_Stream->SM0AR = std::bit_cast<uint32_t>(DmaStreamHandle::bufferOne.data());

            if constexpr (config.mode == DmaMode::double_buffer) {
                m_DMA_Stream->SM1AR = std::bit_cast<uint32_t>(DmaStreamHandle::bufferTwo.data());
            }
            
            m_DMA_Stream->SCR |= ENABLE;
        }

        void handleIRQ() {
            using namespace DMA_SETUP;

            uint32_t SR_REG;

            if constexpr (streamNum >= 4) {
                SR_REG = m_DMA->HISR;
            } else {
                SR_REG = m_DMA->LISR;
            }
            
            if (SR_REG & TCIF[streamNum]) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                if constexpr (streamNum >= 4) {
                    m_DMA->HIFCR = CLEAR_TCF[streamNum];
                } else {
                    m_DMA->LIFCR = CLEAR_TCF[streamNum];
                }

                if constexpr (config.mode == DmaMode::double_buffer) {
                    if (m_taskToNotify != nullptr) {
                        if (m_DMA_Stream->SCR & CURRENT_TARGET) {
                            xTaskNotifyFromISR
                            (
                                m_taskToNotify,
                                std::bit_cast<uint32_t>(bufferOne.data()),
                                eSetValueWithOverwrite,
                                &xHigherPriorityTaskWoken
                            );
                        } else {
                            xTaskNotifyFromISR
                            (
                                m_taskToNotify,
                                std::bit_cast<uint32_t>(bufferTwo.data()),
                                eSetValueWithOverwrite,
                                &xHigherPriorityTaskWoken
                            );
                        }
                    }
                } else if constexpr (config.mode == DmaMode::single) {
                    if (m_taskToNotify != nullptr) {
                        xTaskNotifyFromISR
                        (
                            m_taskToNotify,
                            std::bit_cast<uint32_t>(bufferOne.data()),
                            eSetValueWithOverwrite,
                            &xHigherPriorityTaskWoken
                        );
                    }
                    m_DMA_Stream->SCR |= ENABLE;
                }

                //FreeRTOS macro, NOLINT used - non-user-code
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //NOLINT(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-type-cstyle-cast,hicpp-no-assembler)
            }
        }
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
};

#endif
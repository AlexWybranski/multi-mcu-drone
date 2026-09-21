#include <cstdint>

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
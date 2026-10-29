# Flight Controller (STM32F411)

This module acts as the central flight computer. It is responsible for high-frequency, deterministic tasks: IMU sensor sampling via SPI, PID calculation, motor mixer calculations, and high-speed PWM generation for the ESCs.

---

## Engineering Highlights

Instead of using automated code generators (like CubeMX) or heavy vendor abstraction layers (STM32CubeHAL), this entire firmware is built from the ground up to ensure absolute control over timing and memory footprint.

### 1. Bare-metal drivers
To guarantee maximum control of peripherals, **every single driver has been written completely from scratch by directly manipulating the STM32 hardware registers**. 
* **Custom Boot Sequence:** Developed a startup file and linker script managing physical memory sectors (`.text`, `.data`, `.bss`) and placing the vector table manually.
* **Peripheral Controls:** Custom bare-metal drivers written for **SPI** (configured for high-speed 6 MHz (750 kHz on prototype breadboard config) IMU communication), **UART WITH DMA** (interrupt-driven packet parsing), and **TIM (PWM)** for high-frequency motor duty cycle injection.

### 2. Metaprogramming with C++ Templates for Tailor-Made DMA Interrupts
One of the most advanced architectural features of this firmware is the **compile-time customization of DMA Streams using C++ Templates (`if constexpr`)**, ensuring as low as possible runtime overhead inside critical Interrupt Service Routines (ISRs).

* **The Challenge:** Traditional DMA drivers often use generic runtime `if/else` checks to determine channel status or buffer modes (Single vs Double Buffer), wasting precious CPU cycles during flight loops.
* **The Solution:** Developed a highly optimized `DmaStreamHandle` template class. Hardware layout constants, memory constraints, and scheduling dependencies are passed as template parameters:

```cpp
struct DmaStreamConfig {
    uint32_t dmaBaseAddr;
    uint32_t streamNum;
    DmaChannel channel;
    DmaDirection direction;
    DmaPriority priority;
    DmaMode mode;
    size_t ndtrValue;
};

template<DmaStreamConfig config>
class DmaStreamHandle { ... };
```

* **Minimal-Branching ISR:** Utilizing C++20 Compile-Time Branching (`if constexpr`), the compiler generates a custom-tailored, ISR binary specifically optimized for either Single or Double Buffering.
* **Direct RTOS Task Notification:** When a transfer completes, the ISR bypasses heavy RTOS queues and directly pushes the raw memory buffer pointer (`std::bit_cast<uint32_t>(bufferOne.data())`) straight into the consumer task's notification register using `xTaskNotifyFromISR()`.


### 3. RTOS Task Architecture & Real-Time Scheduling
The system utilizes **FreeRTOS** to handle multi-tasking without sacrificing real-time deterministic performance. Tasks are strictly separated by execution frequency and priority:

| Task Name | Priority | Frequency | Description |
| :--- | :---: | :---: | :--- |
| **enginesTask** | `4` | 2000 Hz | Querying IMU sensor, calculates real and set angles, computes the Motor Mixer and PID mathematics, updates PWM channels. |
| **receiverTask** | `3`  | On demand | Blocked until a new UART frame arrives from the ESP32 receiver. Unpacks the 8-byte packet structure, calculates CRC for first 4 bytes via the hardware CRC peripheral and validates data. |
| **failsafeTask**| `5`  | On demand | If the leaky bucket for CRC calculation is full or packets are not arriving for the set time, the notification is sent to this task. **receiverTask** is being deleted, failsafe takes control over the drone and is securing the safe landing |

---

## Verification & Testing
* **Algorithmic Validation:** The Motor Mixer logic was isolated and tested natively on a host PC using standard console utilities (`std::cout`) to verify axis calculations.
* **Hardware Verification:** Output behaviour is monitored in real-time on the prototype hardware using 4 independent hardware PWM channels driving status LEDs, visually mapping motor responses prior to final ESC integration.

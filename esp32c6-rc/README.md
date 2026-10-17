# Remote control module (ESP32-C6)

This subsystem acts as the transmitter. It handles wireless gamepad inputs and packages them into high-speed, optimized telemetry frames.

### Features
* **C/C++ Interoperability (Bridge Functions):** Integrated a C++ static class with the standard C-based `bluepad32` library. Implemented custom bridge functions (`extern "C"`) to call C++ class functions from `bluepad32` callbacks.
* **Bluepad32 & BTStack Integration:** Interfaces with standard Bluetooth controllers (e.g., PS4/Xbox) via a single-core BLE stack.
* **Data Packing Optimization:** Packs joystick data and CRC32 value into a 64-bit packet to reduce airtime and latency.
* **ESP-NOW Transmission:** Transmits custom packet structure directly to the drone via raw ESP-NOW frames (bypassing heavy Wi-Fi protocol overhead).

## Packet structure
[shared/dronePacket.hpp](shared/dronePacket.hpp)`
```cpp
struct __attribute__((packed)) DroneControlData { // Drone's control data updated and evaluated at runtime.
    uint16_t throttle : 8;
    int16_t roll : 9;
    int16_t pitch : 9;
    uint8_t buttonControlReg : 6; // 6-bit button register to handle status flags (e.g. YAW_LEFT, CAM_UP, START_ENGINES, NO_PAD)
};

struct __attribute__((packed)) DroneControlPacket {
    ControlDataUnion controlData; // Union allows treating DroneControlData both as a raw uint32_t and as a bit-field struct (for std::atomic task synchronization).
    uint32_t crcValue{0xFFFFFFFF}; //initial CRC value that is calculated before sending packet
};
```

# Receiver (on board ESP32-C3)

This subsystem bridges the wireless radio link with the main Flight Controller and handles local servo camera control. To ensure ultra-low latency, the application bypasses complex scheduling—the entire processing pipeline is executed inside a single, optimized ESP-NOW callback event.

### Features
* **Asynchronous Packet Capture:** Listens for ESP-NOW frames from the transmitter in real-time.
* **UART Data Bridge:** Unpacks data and streams control packets directly to the STM32F411 Flight Controller over UART clocked at **115200 bps**.
* **Hardware PWM Generation:** Independently generates hardware PWM signals to control the camera servo, offloading this peripheral task from the main flight computer.

#ifndef RTOS_TASKS_HPP
#define RTOS_TASKS_HPP

#include <cstdint>
#include <cstring>
#include <array> // IWYU pragma: keep
#include <atomic> // IWYU pragma: keep

#include "crc.hpp" // IWYU pragma: keep
#include "dronePacket.hpp" // IWYU pragma: keep
#include "dma.hpp" // IWYU pragma: keep
#include "spi.hpp"
#include "imu.hpp" // IWYU pragma: keep
#include "tim.hpp" // IWYU pragma: keep
#include "motors.hpp" // IWYU pragma: keep
#include "pid.hpp" // IWYU pragma: keep
#include "kiss_telemetry.hpp" // IWYU pragma: keep

extern "C" {
    #include "FreeRTOS.h" // IWYU pragma: keep
    #include "task.h"
}

namespace RTOS_INFO {
    constexpr uint32_t IMU_TASK_STACK_DEPTH = 512;
    constexpr uint32_t RECEIVER_TASK_STACK_DEPTH = 512;
    constexpr uint32_t FLIGHT_CONTROL_TASK_STACK_DEPTH = 512;
    constexpr uint32_t FAILSAFE_TASK_STACK_DEPTH = 512;
    constexpr uint32_t TELEMETRY_TASK_STACK_DEPTH = 256;

    constexpr UBaseType_t RECEIVER_TASK_PRIORITY = 3; 
    constexpr UBaseType_t FLIGHT_CONTROL_TASK_PRIORITY = 4; 
    constexpr UBaseType_t FAILSAFE_TASK_PRIORITY = 5;
    constexpr UBaseType_t TELEMETRY_TASK_PRIORITY = 3;

    //FOR RECEIVER TASK
    constexpr uint32_t MAX_BAD_PACKETS = 5U;
}

struct flightControlTaskContext {
    using DmaStreamManager = void(*)(void);
    DmaStreamManager dmaResetFunc;
    SpiHandle& spi_ptr;
    Tim1Handle& tim_ptr;
};

void initTasks(flightControlTaskContext* flightControlTaskCtx);

void receiverTask(void* pvParameters);
void flightControlTask(void* pvParameters);
void failsafeTask(void* pvParameters);
void telemetryTask(void* pvParameters);

TaskHandle_t getReceiverTaskHandle();
TaskHandle_t getFlightControlTaskHandle();
TaskHandle_t getFailsafeTaskHandle();
TaskHandle_t getTelemetryTaskHandle();

#endif
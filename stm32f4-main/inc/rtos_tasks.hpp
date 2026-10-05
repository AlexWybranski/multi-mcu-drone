#ifndef RTOS_TASKS_HPP
#define RTOS_TASKS_HPP

#include <cstdint>
#include <cstring>
#include <array> // IWYU pragma: keep
#include <atomic> // IWYU pragma: keep

#include "crc.hpp" // IWYU pragma: keep
#include "dronePacket.hpp" // IWYU pragma: keep
#include "control.hpp" // IWYU pragma: keep
#include "dma.hpp"
#include "spi.hpp"
#include "imu.hpp" // IWYU pragma: keep
#include "tim.hpp" // IWYU pragma: keep
#include "motors.hpp" // IWYU pragma: keep

extern "C" {
    #include "FreeRTOS.h" // IWYU pragma: keep
    #include "task.h"
}

namespace RTOS_INFO {
    constexpr uint32_t IMU_TASK_STACK_DEPTH = 512;
    constexpr uint32_t RECEIVER_TASK_STACK_DEPTH = 512;
    constexpr uint32_t ENGINES_TASK_STACK_DEPTH = 512;
    constexpr uint32_t FAILSAFE_TASK_STACK_DEPTH = 512;

    constexpr UBaseType_t IMU_TASK_PRIORITY = 3; 
    constexpr UBaseType_t RECEIVER_TASK_PRIORITY = 4; 
    constexpr UBaseType_t ENGINES_TASK_PRIORITY = 2; 
    constexpr UBaseType_t FAILSAFE_TASK_PRIORITY = 5;

    //RECEIVER TASK
    constexpr uint32_t MAX_BAD_PACKETS = 5U;
}

struct imuTaskContext {
    using DmaStreamManager = void(*)(void);
    DmaStreamManager dmaResetFunc;
    SpiHandle* spi_ptr;
};

struct enginesTaskContext {
    Tim1Handle* tim_ptr;
};

void initTasks(imuTaskContext* imuCtx);

void imuTask(void* pvParameters);
void receiverTask(void* pvParameters);
void enginesTask(void* pvParameters);
void failsafeTask(void* pvParameters);

TaskHandle_t getReceiverTaskHandle();
TaskHandle_t getImuTaskHandle();

#endif
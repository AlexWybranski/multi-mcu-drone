#ifndef RTOS_TASKS_HPP
#define RTOS_TASKS_HPP

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
}

void initTasks();

void imuTask(void* pvParameters);
void receiverTask(void* pvParameters);
void enginesTask(void* pvParameters);
void failsafeTask(void* pvParameters);

TaskHandle_t getReceiverTaskHandle();

#endif
#include "rtos_tasks.hpp"

#include <cstdint>
#include <cstring>
#include <array>

#include "crc.hpp"
#include "dronePacket.hpp"
#include "control.hpp"
#include "dma.hpp"

//Anonymous namespace guarantees that this variables are visible only within this file
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace {
    std::array<StackType_t, RTOS_INFO::IMU_TASK_STACK_DEPTH> imuTaskStack;
    std::array<StackType_t, RTOS_INFO::RECEIVER_TASK_STACK_DEPTH> receiverTaskStack;
    std::array<StackType_t, RTOS_INFO::ENGINES_TASK_STACK_DEPTH> enginesTaskStack;
    std::array<StackType_t, RTOS_INFO::FAILSAFE_TASK_STACK_DEPTH> failsafeTaskStack;

    StaticTask_t imuTaskBuffer;
    StaticTask_t receiverTaskBuffer;
    StaticTask_t enginesTaskBuffer;
    StaticTask_t failsafeTaskBuffer;

    TaskHandle_t imuTaskHandle = nullptr;
    TaskHandle_t receiverTaskHandle = nullptr;
    TaskHandle_t enginesTaskHandle = nullptr;
    TaskHandle_t failsafeTaskHandle = nullptr;
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

TaskHandle_t getReceiverTaskHandle() {
    return receiverTaskHandle;
}

void initTasks() {
    imuTaskHandle = xTaskCreateStatic
    (
        imuTask,
        "imuTask",
        RTOS_INFO::IMU_TASK_STACK_DEPTH,
        nullptr,
        RTOS_INFO::IMU_TASK_PRIORITY,
        imuTaskStack.data(),
        &imuTaskBuffer
    );
    
    receiverTaskHandle = xTaskCreateStatic
    (
        receiverTask,
        "receiverTask",
        RTOS_INFO::RECEIVER_TASK_STACK_DEPTH,
        nullptr,
        RTOS_INFO::RECEIVER_TASK_PRIORITY,
        receiverTaskStack.data(),
        &receiverTaskBuffer
    );
    
    enginesTaskHandle = xTaskCreateStatic
    (
        enginesTask,
        "enginesTask",
        RTOS_INFO::ENGINES_TASK_STACK_DEPTH,
        nullptr,
        RTOS_INFO::ENGINES_TASK_PRIORITY,
        enginesTaskStack.data(),
        &enginesTaskBuffer
    );
    
    failsafeTaskHandle = xTaskCreateStatic
    (
        failsafeTask,
        "failsafeTask",
        RTOS_INFO::FAILSAFE_TASK_STACK_DEPTH,
        nullptr,
        RTOS_INFO::FAILSAFE_TASK_PRIORITY,
        failsafeTaskStack.data(),
        &failsafeTaskBuffer
    );
}

void imuTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    while(1) {
        vTaskDelay(100);
    }
}

void receiverTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    uint32_t rawDataAddress{0};
    uint8_t* rawDataPtr{nullptr};
    
    uint32_t controlData{0};
    DroneControlPacket receivedPacket{};
    uint32_t crc{0};
    uint8_t badPacketsCounter{0};
    bool isInitialized{false};

    while(1) {
        if (xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &rawDataAddress,
            pdMS_TO_TICKS(1000)
        ) == pdPASS) {
            rawDataPtr = reinterpret_cast<uint8_t*>(rawDataAddress);
            std::memcpy(&receivedPacket, rawDataPtr, DMA_SETUP::NDTR_VAL);

            std::memcpy(&controlData, &receivedPacket, packetStructure::DATA_LENGTH);

            crc = calculateCRC(controlData);

            if (crc == receivedPacket.crcValue) {
                if (!isInitialized) {
                    isInitialized = true;
                }
                badPacketsCounter = 0;
                //update global control
            } else {
                badPacketsCounter++;
                if (badPacketsCounter > 5) {
                    xTaskNotifyGive(failsafeTaskHandle);
                }
            }

            crc = 0U;
            rawDataAddress = 0U;
        } else {
            if (isInitialized) {
                xTaskNotifyGive(failsafeTaskHandle);
            }
        }
    }
}

void enginesTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    while(1) {
        vTaskDelay(100);
    }
}

void failsafeTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    //ensure that only failsafe task is able to operate engines
    //vTaskDelete(enginesTaskHandle);
    //vTaskDelete(receiverTaskHandle);

    while(1) {
        vTaskDelay(100);
    }
}
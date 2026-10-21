#include "rtos_tasks.hpp"

//Anonymous namespace guarantees that this variables are visible only within this file
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace {
    std::array<StackType_t, RTOS_INFO::RECEIVER_TASK_STACK_DEPTH> receiverTaskStack;
    std::array<StackType_t, RTOS_INFO::FLIGHT_CONTROL_TASK_STACK_DEPTH> flightControlTaskStack;
    std::array<StackType_t, RTOS_INFO::FAILSAFE_TASK_STACK_DEPTH> failsafeTaskStack;

    StaticTask_t receiverTaskBuffer;
    StaticTask_t flightControlTaskBuffer;
    StaticTask_t failsafeTaskBuffer;

    TaskHandle_t receiverTaskHandle = nullptr;
    TaskHandle_t flightControlTaskHandle = nullptr;
    TaskHandle_t failsafeTaskHandle = nullptr;
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

TaskHandle_t getReceiverTaskHandle() {
    return receiverTaskHandle;
}

TaskHandle_t getFlightControlTaskHandle() {
    return flightControlTaskHandle;
}

TaskHandle_t getFailsafeTaskHandle() {
    return failsafeTaskHandle;
}

//global variable secured by atomic and critical sections
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace {
    std::atomic<uint32_t> rawPacket{0};
    std::atomic<bool> g_hasLanded{false};
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void initTasks(flightControlTaskContext* flightControlTaskCtx) {
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
    
    flightControlTaskHandle = xTaskCreateStatic
    (
        flightControlTask,
        "enginesTask",
        RTOS_INFO::FLIGHT_CONTROL_TASK_STACK_DEPTH,
        flightControlTaskCtx,
        RTOS_INFO::FLIGHT_CONTROL_TASK_PRIORITY,
        flightControlTaskStack.data(),
        &flightControlTaskBuffer
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

void receiverTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    uint32_t rawDataAddress{0};
    uint8_t* rawDataPtr{nullptr};

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
            rawDataPtr = std::bit_cast<uint8_t*>(rawDataAddress);
            
            taskENTER_CRITICAL();
                std::memcpy(&receivedPacket, rawDataPtr, DMA_SETUP::UART_NDTR_VAL);
            taskEXIT_CRITICAL();

            crc = calculateCRC(receivedPacket.controlData.dataRaw);

            if (crc == receivedPacket.crcValue) {
                if (!isInitialized) {
                    isInitialized = true;
                }
                badPacketsCounter = (badPacketsCounter > 0) ? badPacketsCounter - 1 : 0;

                if ((receivedPacket.controlData.data.buttonControlReg & ControlRegister::NO_PAD) && isInitialized) {
                    xTaskNotifyGive(failsafeTaskHandle);
                }

                //update global control
                rawPacket.store(receivedPacket.controlData.dataRaw);
                
            } else {
                badPacketsCounter++;
                if (badPacketsCounter > RTOS_INFO::MAX_BAD_PACKETS) {
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

void flightControlTask(void* pvParameters) {
    auto* ctx = static_cast<flightControlTaskContext*>(pvParameters);
    auto& spi = ctx->spi_ptr;
    auto& tim = ctx->tim_ptr;

    std::array<uint8_t, DMA_SETUP::SPI_NDTR_VAL> imuData{};
    uint8_t* imuDataStartPtr = &imuData[1];
    ImuTelemetry imuDataRaw{};
    ImuDataNormalized imuDataNormalized{};
    uint32_t dataAddress = 0U;

    DroneControlData packet{};

    Pid pid;
    MotorPWMs motors{};

    TickType_t xLastWakeTime;

    bool engineArmed = false;
    bool hasLanded = false;

    if(!initImu(spi)) {
        return;
    } 

    while(1) {
        
        ctx->dmaResetFunc();
        
        spi.DMAstartTransfer();
        
        xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &dataAddress,
            portMAX_DELAY
        );
        
        spi.DMAendTransfer();
        
        packet = std::bit_cast<DroneControlData>(rawPacket.load());
        
        std::memcpy(imuData.data(), std::bit_cast<uint8_t*>(dataAddress), LSM6DS3::SENSOR_DATA_READ_SEQUENCE_LENGTH);
        
        std::memcpy(&imuDataRaw, imuDataStartPtr, sizeof(ImuTelemetry));

        normalizeDataFromImu(imuDataRaw, imuDataNormalized);

        pid.calculateAxisValuesGyro(imuDataNormalized);
        pid.calculateAxisValuesAcc(imuDataNormalized);

        pid.complementaryFilter();

        hasLanded = g_hasLanded.load();

        motors = MotorMixer::mixMotors(packet);

        if ((packet.buttonControlReg & ControlRegister::START_STOP_ENGINE) && engineArmed == false) {
            engineArmed = true;
        } else if (((packet.buttonControlReg & ControlRegister::START_STOP_ENGINE) || hasLanded) && engineArmed) {
            engineArmed = false;
        }

        if(engineArmed) {
            tim.setDutyCh1(motors.motorRigFro_1);
            tim.setDutyCh2(motors.motorLefRea_2);
            tim.setDutyCh3(motors.motorLefFro_3);
            tim.setDutyCh4(motors.motorRigRea_4);
        } else {
            tim.setDutyCh1(0);
            tim.setDutyCh2(0);
            tim.setDutyCh3(0);
            tim.setDutyCh4(0);
        }

        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
    }
}

void failsafeTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    ControlDataUnion failsafeControl;

    bool hasLanded = false;
    
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
    taskENTER_CRITICAL();
        vTaskDelete(receiverTaskHandle);
    taskEXIT_CRITICAL();

    while(1) {
        for (uint8_t i = 128U; i > 0; i--) {
            failsafeControl.data.pitch = 0;
            failsafeControl.data.roll = 0;
            failsafeControl.data.throttle = i;
            failsafeControl.data.buttonControlReg = (0 | ControlRegister::NO_PAD);
            
            rawPacket.store(failsafeControl.dataRaw);

            vTaskDelay(pdMS_TO_TICKS(10));
        }
        hasLanded = true;
        g_hasLanded.store(hasLanded);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
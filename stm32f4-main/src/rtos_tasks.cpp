#include "rtos_tasks.hpp"

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

TaskHandle_t getImuTaskHandle() {
    return imuTaskHandle;
}

//global variables
namespace {
    std::atomic<uint32_t> rawPacket{0};
    ImuTelemetry g_imuData;
}

void initTasks(imuTaskContext* imuCtx, enginesTaskContext* enginesTaskCtx) {
    imuTaskHandle = xTaskCreateStatic
    (
        imuTask,
        "imuTask",
        RTOS_INFO::IMU_TASK_STACK_DEPTH,
        imuCtx,
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
        enginesTaskCtx,
        RTOS_INFO::ENGINES_TASK_PRIORITY,
        enginesTaskStack.data(),
        &enginesTaskBuffer
    );
    
    failsafeTaskHandle = xTaskCreateStatic
    (
        failsafeTask,
        "failsafeTask",
        RTOS_INFO::FAILSAFE_TASK_STACK_DEPTH,
        enginesTaskCtx,
        RTOS_INFO::FAILSAFE_TASK_PRIORITY,
        failsafeTaskStack.data(),
        &failsafeTaskBuffer
    );
}

void imuTask(void* pvParameters) {
    auto* ctx = static_cast<imuTaskContext*>(pvParameters);

    auto* spi = ctx->spi_ptr;

    std::array<uint8_t, DMA_SETUP::SPI_NDTR_VAL> imuData{};

    uint8_t* imuDataStartPtr = &imuData[1];

    spi->setCsHigh();
    vTaskDelay(pdMS_TO_TICKS(5));
    spi->setCsLow();
    vTaskDelay(pdMS_TO_TICKS(5));
    spi->setCsHigh();
    vTaskDelay(pdMS_TO_TICKS(5));

    spi->POLLread_write(LSM6DS3::sanityCheck.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, false);
    spi->POLLread_write(LSM6DS3::ctrl3_c_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, false);
    spi->POLLread_write(LSM6DS3::ctrl1_xl_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, false);
    spi->POLLread_write(LSM6DS3::ctrl2_g_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, false);

    uint32_t dataAddress = 0U;

    spi->POLLread_write(LSM6DS3::dataReadSequence.data(), imuData.data(), LSM6DS3::SENSOR_DATA_READ_SEQUENCE_LENGTH, false);

    while(1) {
        ctx->dmaResetFunc();
        
        spi->DMAstartTransfer();

        xTaskNotifyWait(
                0,
                0xFFFFFFFF,
                &dataAddress,
                portMAX_DELAY
        );

        spi->DMAendTransfer();

        std::memcpy(imuData.data(), reinterpret_cast<uint8_t*>(dataAddress), LSM6DS3::SENSOR_DATA_READ_SEQUENCE_LENGTH);

        dataAddress = 0;

        taskENTER_CRITICAL();
            std::memcpy(&g_imuData, imuDataStartPtr, sizeof(ImuTelemetry));
        taskEXIT_CRITICAL();


        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void receiverTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    uint32_t rawDataAddress{0};
    uint8_t* rawDataPtr{nullptr};

    DroneControlPacket receivedPacket{};
    ControlDataUnion dataUnion{};
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
            std::memcpy(&receivedPacket, rawDataPtr, DMA_SETUP::UART_NDTR_VAL);

            std::memcpy(&dataUnion, &receivedPacket, packetStructure::DATA_LENGTH);

            crc = calculateCRC(dataUnion.dataRaw);

            if (crc == receivedPacket.crcValue) {
                if (!isInitialized) {
                    isInitialized = true;
                }
                badPacketsCounter = (badPacketsCounter > 0) ? badPacketsCounter - 1 : 0;

                //update global control
                rawPacket.store(dataUnion.dataRaw);

            } else if ((receivedPacket.controlData.data.buttonControlReg & ControlRegister::NO_PAD) && isInitialized) {
                xTaskNotifyGive(failsafeTaskHandle);
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

void enginesTask(void* pvParameters) {
    auto* ctx = static_cast<enginesTaskContext*>(pvParameters);

    auto* tim = ctx->tim_ptr;

    DroneControlData packet{};

    MotorPWMs motors{};

    ImuTelemetry imuData{};

    while(1) {
        packet = std::bit_cast<DroneControlData>(rawPacket.load());

        taskENTER_CRITICAL();
            imuData = g_imuData;
        taskEXIT_CRITICAL();

        motors = MotorMixer::mixMotors(packet);

        tim->setDutyCh1(motors.motorRigFro_1);
        tim->setDutyCh2(motors.motorLefRea_2);
        tim->setDutyCh3(motors.motorLefFro_3);
        tim->setDutyCh4(motors.motorRigRea_4);

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void failsafeTask(void* pvParameters) {
    auto* ctx = static_cast<enginesTaskContext*>(pvParameters);

    auto* tim = ctx->tim_ptr;

    MotorPWMs motors{};

    bool stationary = false;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    taskENTER_CRITICAL();
        //ensure that only failsafe task is able to operate engines
        vTaskDelete(enginesTaskHandle);
        vTaskDelete(receiverTaskHandle);
    taskEXIT_CRITICAL();

    while(1) {
        while (!stationary) {
            for (int16_t i = 128; i > 0; i--) {
                motors.motorRigFro_1 = i;
                motors.motorLefRea_2 = i;
                motors.motorLefFro_3 = i;
                motors.motorRigRea_4 = i;
    
                tim->setDutyCh1(motors.motorRigFro_1);
                tim->setDutyCh2(motors.motorLefRea_2);
                tim->setDutyCh3(motors.motorLefFro_3);
                tim->setDutyCh4(motors.motorRigRea_4);
    
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            stationary = true;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
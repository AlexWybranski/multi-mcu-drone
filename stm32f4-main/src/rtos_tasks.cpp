#include "rtos_tasks.hpp"

//Anonymous namespace guarantees that this variables are visible only within this file
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace {
    std::array<StackType_t, RTOS_INFO::RECEIVER_TASK_STACK_DEPTH> receiverTaskStack;
    std::array<StackType_t, RTOS_INFO::FLIGHT_CONTROL_TASK_STACK_DEPTH> flightControlTaskStack;
    std::array<StackType_t, RTOS_INFO::FAILSAFE_TASK_STACK_DEPTH> failsafeTaskStack;
    std::array<StackType_t, RTOS_INFO::TELEMETRY_TASK_STACK_DEPTH> telemetryTaskStack;

    StaticTask_t receiverTaskBuffer;
    StaticTask_t flightControlTaskBuffer;
    StaticTask_t failsafeTaskBuffer;
    StaticTask_t telemetryTaskBuffer;

    TaskHandle_t receiverTaskHandle = nullptr;
    TaskHandle_t flightControlTaskHandle = nullptr;
    TaskHandle_t failsafeTaskHandle = nullptr;
    TaskHandle_t telemetryTaskHandle = nullptr;
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

// DEBUG
uint16_t duty_m1{}, duty_m2{}, duty_m3{}, duty_m4{};
uint16_t debug_engineArm{};
uint16_t debug_throttle{};
int16_t debug_pitch{};
int16_t debug_roll{};
uint8_t debug_buttonReg{};
uint16_t debug_batteryLevel{};

TaskHandle_t getReceiverTaskHandle() {
    return receiverTaskHandle;
}

TaskHandle_t getFlightControlTaskHandle() {
    return flightControlTaskHandle;
}

TaskHandle_t getFailsafeTaskHandle() {
    return failsafeTaskHandle;
}

TaskHandle_t getTelemetryTaskHandle() {
    return telemetryTaskHandle;
}

//global variable secured by atomic and critical sections
//NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
namespace {
    std::atomic<uint32_t> rawPacket{0U};
    std::atomic<bool> g_hasLanded{false};
}
//NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void initTasks(flightControlTaskContext* flightControlTaskCtx, receiverTaskContext* receiverTaskCtx) {
    receiverTaskHandle = xTaskCreateStatic
    (
        receiverTask,
        "receiverTask",
        RTOS_INFO::RECEIVER_TASK_STACK_DEPTH,
        receiverTaskCtx,
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

    telemetryTaskHandle = xTaskCreateStatic(
        telemetryTask,
        "telemetryTask",
        RTOS_INFO::TELEMETRY_TASK_STACK_DEPTH,
        nullptr,
        RTOS_INFO::TELEMETRY_TASK_PRIORITY,
        telemetryTaskStack.data(),
        &telemetryTaskBuffer
    );
}

void receiverTask(void* pvParameters) {
    auto* ctx = static_cast<receiverTaskContext*>(pvParameters);

    auto& gpio = ctx->gpio_ptr;

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

            crc = calculateCRC(std::bit_cast<uint32_t>(receivedPacket.controlData));

            if (crc == receivedPacket.crcValue) {
                if (!isInitialized) {
                    isInitialized = true;
                    gpio.setPinState(false, GPIOC_PINS::USER_DIODE);
                }
                badPacketsCounter = (badPacketsCounter > 0) ? badPacketsCounter - 1 : 0;

                if ((receivedPacket.controlData.buttonControlReg & ControlRegister::NO_PAD) && isInitialized) {
                    xTaskNotifyGive(failsafeTaskHandle);
                }

                //update global control packet
                rawPacket.store(std::bit_cast<uint32_t>(receivedPacket.controlData));
                
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
    auto& m1 = ctx->m1_ptr;
    auto& m2 = ctx->m2_ptr;
    auto& m3 = ctx->m3_ptr;
    auto& m4 = ctx->m4_ptr;

    std::array<uint8_t, DMA_SETUP::SPI_NDTR_VAL> imuData{};
    uint8_t* imuDataStartPtr = &imuData[1];
    ImuTelemetry imuDataRaw{};
    ImuDataNormalized imuDataNormalized{};
    uint32_t dataAddress = 0U;

    DroneControlData packet{};

    Pid pid;
    MotorPWMs motors{};

    TickType_t xLastWakeTime = xTaskGetTickCount();

    bool engineArmed = false;
    bool hasLanded = false;

    if(!initImu(spi)) {
        return;
    } 

    while(1) {
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2));

        ctx->dmaResetFunc();
        
        spi.DMAstartTransfer();
        
        xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &dataAddress,
            portMAX_DELAY
        );
        
        spi.DMAendTransfer();
        
        taskENTER_CRITICAL();
            std::memcpy(imuData.data(), std::bit_cast<uint8_t*>(dataAddress), LSM6DS3::SENSOR_DATA_READ_SEQUENCE_LENGTH);
        taskEXIT_CRITICAL();

        packet = std::bit_cast<DroneControlData>(rawPacket.load());

        debug_throttle = packet.throttle;
        debug_roll = packet.roll;
        debug_pitch = packet.pitch;
        debug_buttonReg = packet.buttonControlReg;

        std::memcpy(&imuDataRaw, imuDataStartPtr, sizeof(ImuTelemetry));

        normalizeDataFromImu(imuDataRaw, imuDataNormalized);

        pid.calculateAxisValuesGyro(imuDataNormalized);
        pid.calculateAxisValuesAcc(imuDataNormalized);

        pid.complementaryFilter();

        pid.inputToAngle(packet.roll, packet.pitch);

        hasLanded = g_hasLanded.load();

        pid.calculatePID();

        MotorMixer::mixMotors(packet, pid.pidOutput, motors);

        if ((packet.buttonControlReg & ControlRegister::START_STOP_ENGINE) && engineArmed == false) {
            engineArmed = true;
            pid.resetIntegrals();
        } else if (((packet.buttonControlReg & ControlRegister::START_STOP_ENGINE) /*&& hasLanded*/) && engineArmed) {
            engineArmed = false;
            pid.resetIntegrals();
        } else if (((packet.buttonControlReg & ControlRegister::NO_PAD) && hasLanded) && engineArmed) {
            engineArmed = false;
            pid.resetIntegrals();
        }

        debug_engineArm = static_cast<uint16_t>(static_cast<uint8_t>(engineArmed));

        tim.disableTimDShot();
        if(engineArmed) {
            m1.assembleDshotBuffer(motors.motor_1, false);
            m2.assembleDshotBuffer(motors.motor_2, false);
            m3.assembleDshotBuffer(motors.motor_3, false);
            m4.assembleDshotBuffer(motors.motor_4, false);
            // DEBUG
            duty_m1 = motors.motor_1;
            duty_m2 = motors.motor_2;
            duty_m3 = motors.motor_3;
            duty_m4 = motors.motor_4;
        } else {
            m1.assembleDshotBuffer(0U, false);
            m2.assembleDshotBuffer(0U, false);
            m3.assembleDshotBuffer(0U, false);
            m4.assembleDshotBuffer(0U, false);

            duty_m1 = 0U;
            duty_m2 = 0U;
            duty_m3 = 0U;
            duty_m4 = 0U;
        }
        m1.setupTransfer();
        m2.setupTransfer();
        m3.setupTransfer();
        m4.setupTransfer();
        m1.startTransfer();
        m2.startTransfer();
        m3.startTransfer();
        m4.startTransfer();
        tim.enableTimDShot();
    }
}

void failsafeTask(void* pvParameters) {
    static_cast<void>(pvParameters);

    DroneControlData failsafeControl{};

    bool hasLanded = false;
    
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
    taskENTER_CRITICAL();
        vTaskDelete(receiverTaskHandle);
        vTaskDelete(telemetryTaskHandle);
    taskEXIT_CRITICAL();

    while(1) {
        for (uint8_t i = 128U; i > 0; i--) {
            failsafeControl.pitch = 0;
            failsafeControl.roll = 0;
            failsafeControl.throttle = i;
            failsafeControl.buttonControlReg = (0U | ControlRegister::NO_PAD);
            
            rawPacket.store(std::bit_cast<uint32_t>(failsafeControl));
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        hasLanded = true;
        g_hasLanded.store(hasLanded);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void telemetryTask(void *pvParameters) {
    static_cast<void>(pvParameters);

    KissTelemetry escTelemetry{};
    uint32_t rawDataAddress{};
    uint8_t* rawDataPtr{};
    uint16_t batteryLevel{};

    while (1) {
        if (xTaskNotifyWait(
            0,
            0xFFFFFFFF,
            &rawDataAddress,
            pdMS_TO_TICKS(1000)
        ) == pdPASS) {
            rawDataPtr = std::bit_cast<uint8_t*>(rawDataAddress);

            taskENTER_CRITICAL();
                std::memcpy(&escTelemetry, rawDataPtr, sizeof(KissTelemetry));
            taskEXIT_CRITICAL();

            batteryLevel = static_cast<uint16_t>((escTelemetry.voltage_h << 8U) | (escTelemetry.voltage_l << 0U));
            debug_batteryLevel = batteryLevel;

            if (batteryLevel < 1450) {
                xTaskNotifyGive(failsafeTaskHandle);
            }
        }
    }
}
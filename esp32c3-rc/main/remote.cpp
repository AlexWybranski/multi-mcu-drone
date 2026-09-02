#include "remote.hpp"
#include "bridge.h"

void RemoteControl::handlePadData(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons) {
    using namespace ButtonMasks;

    DroneControlPacket dronePacket{0,0,0,0,0};

    int32_t ay = applyDeadzone(axis_y);
    int32_t arx = applyDeadzone(axis_rx);
    int32_t ary = applyDeadzone(axis_ry);

    dronePacket.throttle = scaleAxisValue(ay);
    dronePacket.roll = scaleAxisValue(arx);
    dronePacket.pitch = scaleAxisValue(ary);
        
    if (buttons & BUTTON_SHOULDER_L) {
        dronePacket.buttonControlReg |= (0b1U << Buttons::YAW_LEFT);
    }

    if (buttons & BUTTON_SHOULDER_R) {
        dronePacket.buttonControlReg |= (0b1U << Buttons::YAW_RIGHT);
    }

    if (buttons & BUTTON_A) {
        dronePacket.buttonControlReg |= (0b1U << Buttons::START_STOP_ENGINE);
    }

    calculateCRC(&dronePacket);
    sendPacket(dronePacket);
} 

void call_remote_control(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons) {
    RemoteControl::handlePadData(axis_y, axis_rx, axis_ry, buttons);
}
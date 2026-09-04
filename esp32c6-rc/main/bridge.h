#ifndef BRIDGE_H
#define BRIDGE_H
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif
    void call_remote_control(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad);
    void call_pad_disconnected();
    void call_pad_ready();
#if __cplusplus
}
#endif

#endif
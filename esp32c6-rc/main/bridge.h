#ifndef BRIDGE_H
#define BRIDGE_H
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif
    void call_remote_control(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad);
#if __cplusplus
}
#endif

#endif
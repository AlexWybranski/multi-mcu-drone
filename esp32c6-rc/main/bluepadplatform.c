// Example file - Public Domain
// Need help? https://tinyurl.com/bluepad32-help

#include "uni_log.h"
#include <stdint.h>
#include <string.h>

#include <uni.h>
#include "bridge.h"

const char* platform_name = "RemoteControl";

// Custom "instance"
typedef struct platform_instance_s {
    uni_gamepad_seat_t gamepad_seat;  // which "seat" is being used
} platform_instance_t;

// Declarations
static void trigger_event_on_gamepad(uni_hid_device_t* d);
static platform_instance_t* get_platform_instance(uni_hid_device_t* d);

//
// Platform Overrides
//
static void platform_init(int argc, const char** argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
}

static void platform_on_init_complete(void) {
    // Safe to call "unsafe" functions since they are called from BT thread

    // Start scanning
    uni_bt_start_scanning_and_autoconnect_unsafe();
    uni_bt_allow_incoming_connections(true);

    // Based on runtime condition, you can delete or list the stored BT keys.
    if (1)
        uni_bt_del_keys_unsafe();
    else
        uni_bt_list_keys_unsafe();
}

static uni_error_t platform_on_device_discovered(bd_addr_t addr, const char* name, uint16_t cod, uint8_t rssi) {
    // You can filter discovered devices here.
    // Just return any value different from UNI_ERROR_SUCCESS;
    // @param addr: the Bluetooth address
    // @param name: could be NULL, could be zero-length, or might contain the name.
    // @param cod: Class of Device. See "uni_bt_defines.h" for possible values.
    // @param rssi: Received Signal Strength Indicator (RSSI) measured in dBms. The higher (255) the better.

    // As an example, if you want to filter out keyboards, do:
    if (((cod & UNI_BT_COD_MINOR_MASK) & UNI_BT_COD_MINOR_KEYBOARD) == UNI_BT_COD_MINOR_KEYBOARD) {
        return UNI_ERROR_IGNORE_DEVICE;
    }

    return UNI_ERROR_SUCCESS;
}

static void platform_on_device_connected(uni_hid_device_t* d) {
    logi("%s: device connected: %p\n", platform_name, d);
}

static void platform_on_device_disconnected(uni_hid_device_t* d) {
    call_pad_disconnected();
}

static uni_error_t platform_on_device_ready(uni_hid_device_t* d) {
    logi("%s: device ready: %p\n", platform_name, d);
    platform_instance_t* ins = get_platform_instance(d);
    ins->gamepad_seat = GAMEPAD_SEAT_A;

    trigger_event_on_gamepad(d);

    call_pad_ready();
    
    return UNI_ERROR_SUCCESS;
}

static void platform_on_controller_data(uni_hid_device_t* d, uni_controller_t* ctl) {
    uni_gamepad_t* gp;

    if(ctl->klass == UNI_CONTROLLER_CLASS_GAMEPAD) {
        gp = &ctl->gamepad;
    
        call_remote_control(gp->axis_y, gp->axis_rx, gp->axis_ry, gp->buttons, gp->dpad);
    }
}

static const uni_property_t* platform_get_property(uni_property_idx_t idx) {
    ARG_UNUSED(idx);
    return NULL;
}

static void platform_on_oob_event(uni_platform_oob_event_t event, void* data) {
    switch (event) {
        case UNI_PLATFORM_OOB_GAMEPAD_SYSTEM_BUTTON: {
            uni_hid_device_t* d = data;

            if (d == NULL) {
                loge("ERROR: platform_on_oob_event: Invalid NULL device\n");
                return;
            }
            logi("%s: on_device_oob_event(): %d\n", platform_name, event);

            platform_instance_t* ins = get_platform_instance(d);
            ins->gamepad_seat = ins->gamepad_seat == GAMEPAD_SEAT_A ? GAMEPAD_SEAT_B : GAMEPAD_SEAT_A;

            trigger_event_on_gamepad(d);
            break;
        }

        case UNI_PLATFORM_OOB_BLUETOOTH_ENABLED:
            logi("%s: Bluetooth enabled: %d\n", platform_name, (bool)(data));
            break;

        default:
            logi("my_platform_on_oob_event: unsupported event: 0x%04x\n", event);
            break;
    }
}

//
// Helpers
//
static platform_instance_t* get_platform_instance(uni_hid_device_t* d) {
    return (platform_instance_t*)&d->platform_data[0];
}

static void trigger_event_on_gamepad(uni_hid_device_t* d) {
    platform_instance_t* ins = get_platform_instance(d);

    if (d->report_parser.play_dual_rumble != NULL) {
        d->report_parser.play_dual_rumble(d, 0 /* delayed start ms */, 150 /* duration ms */, 128 /* weak magnitude */,
                                          40 /* strong magnitude */);
    }

    if (d->report_parser.set_player_leds != NULL) {
        d->report_parser.set_player_leds(d, ins->gamepad_seat);
    }

    if (d->report_parser.set_lightbar_color != NULL) {
        uint8_t red = (ins->gamepad_seat & 0x01) ? 0xff : 0;
        uint8_t green = (ins->gamepad_seat & 0x02) ? 0xff : 0;
        uint8_t blue = (ins->gamepad_seat & 0x04) ? 0xff : 0;
        d->report_parser.set_lightbar_color(d, red, green, blue);
    }
}

//
// Entry Point
//
struct uni_platform* get_my_platform(void) {
    static struct uni_platform plat = {
        .name = "RemoteControl",
        .init = platform_init,
        .on_init_complete = platform_on_init_complete,
        .on_device_discovered = platform_on_device_discovered,
        .on_device_connected = platform_on_device_connected,
        .on_device_disconnected = platform_on_device_disconnected,
        .on_device_ready = platform_on_device_ready,
        .on_oob_event = platform_on_oob_event,
        .on_controller_data = platform_on_controller_data,
        .get_property = platform_get_property,
    };

    return &plat;
}

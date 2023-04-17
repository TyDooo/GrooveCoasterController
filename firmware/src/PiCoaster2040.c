#include "PiCoaster2040.h"
#include "tusb.h"
#include "usb/usb_descriptors.h"
#include "buttons.h"

hid_gamepad_report_t report = {0};

void setup()
{
    tud_init(BOARD_TUD_RHPORT);

    init_buttons();
}

void update()
{
    tud_task();

    if (!tud_ready())
        return;

    update_dpad(&report);
    update_buttons(&report);

    tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

    sleep_ms(10);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize)
{
    // not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen)
{
    // not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// static uint8_t get_gamepad_hat(bool up, bool down, bool left, bool right)
// {
//     switch ((up << 3) | (down << 2) | (left << 1) | right)
//     {
//     case 0b1000:
//         return GAMEPAD_HAT_UP;
//     case 0b0100:
//         return GAMEPAD_HAT_DOWN;
//     case 0b0010:
//         return GAMEPAD_HAT_LEFT;
//     case 0b0001:
//         return GAMEPAD_HAT_RIGHT;
//     case 0b1010:
//         return GAMEPAD_HAT_UP_LEFT;
//     case 0b1001:
//         return GAMEPAD_HAT_UP_RIGHT;
//     case 0b0110:
//         return GAMEPAD_HAT_DOWN_LEFT;
//     case 0b0101:
//         return GAMEPAD_HAT_DOWN_RIGHT;
//     default:
//         return GAMEPAD_HAT_CENTERED;
//     }
// }
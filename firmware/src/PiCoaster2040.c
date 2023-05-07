#include "PiCoaster2040.h"
#include "tusb.h"
#include "usb/usb_descriptors.h"
#include "booster.h"
#include "pinmap.h"
#include "input_state.h"
#include "ws2812.h"

// TODO: move to config
#define WS2812_PIN 21

booster_state_t booster_left = {0};
booster_state_t booster_right = {0};

void setup()
{
    tud_init(BOARD_TUD_RHPORT);

    init_booster(&booster_left, &booster_l_conf);
    init_booster(&booster_right, &booster_r_conf);

    ws2812_init(WS2812_PIN, false);

    set_all_pixels(5, 5, 5);
    show_pixels();
}

void update()
{
    tud_task();

    update_booster_state(&booster_left);
    update_booster_state(&booster_right);

    if (!tud_ready())
        return;

    hid_gamepad_report_t report = get_report(&booster_left, &booster_right);
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

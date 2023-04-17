#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <stdint.h>
#include <stdbool.h>
#include "tusb.h"

typedef struct
{
    uint8_t gpio_pin;
    uint32_t gpio_mask;
    uint32_t last_change;
    bool active;
} button_t;

typedef struct
{
    button_t up;
    button_t down;
    button_t left;
    button_t right;
} dpad_t;

void init_buttons();
void update_dpad(hid_gamepad_report_t *report);
void update_buttons(hid_gamepad_report_t *report);

#endif
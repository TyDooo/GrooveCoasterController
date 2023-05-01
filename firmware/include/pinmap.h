#ifndef _PINMAP_H_
#define _PINMAP_H_

#include "booster.h"
#include "tusb.h"

booster_conf_t booster_l_conf = {
    .up = {.gpio_pin = 15},    // no mask, uses dpad
    .down = {.gpio_pin = 14},  // no mask, uses dpad
    .left = {.gpio_pin = 13},  // no mask, uses dpad
    .right = {.gpio_pin = 12}, // no mask, uses dpad
    .btn = {.gpio_pin = 11, .gpio_mask = GAMEPAD_BUTTON_TL},
};

booster_conf_t booster_r_conf = {
    .up = {.gpio_pin = 16, .gpio_mask = GAMEPAD_BUTTON_NORTH},
    .down = {.gpio_pin = 17, .gpio_mask = GAMEPAD_BUTTON_SOUTH},
    .left = {.gpio_pin = 18, .gpio_mask = GAMEPAD_BUTTON_WEST},
    .right = {.gpio_pin = 19, .gpio_mask = GAMEPAD_BUTTON_EAST},
    .btn = {.gpio_pin = 20, .gpio_mask = GAMEPAD_BUTTON_TR},
};

#endif
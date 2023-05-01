#include "input_state.h"

static uint8_t map_gamepad_hat(booster_btn_t up, booster_btn_t down, booster_btn_t left, booster_btn_t right)
{
    switch ((up.active << 3) | (down.active << 2) | (left.active << 1) | right.active)
    {
    case 0b1000:
        return GAMEPAD_HAT_UP;
    case 0b0100:
        return GAMEPAD_HAT_DOWN;
    case 0b0010:
        return GAMEPAD_HAT_LEFT;
    case 0b0001:
        return GAMEPAD_HAT_RIGHT;
    case 0b1010:
        return GAMEPAD_HAT_UP_LEFT;
    case 0b1001:
        return GAMEPAD_HAT_UP_RIGHT;
    case 0b0110:
        return GAMEPAD_HAT_DOWN_LEFT;
    case 0b0101:
        return GAMEPAD_HAT_DOWN_RIGHT;
    default:
        return GAMEPAD_HAT_CENTERED;
    }
}

static inline void update_button_mask(hid_gamepad_report_t *report, booster_btn_t btn)
{
    report->buttons = btn.active ? (report->buttons | (1 << btn.gpio_mask))
                                 : (report->buttons & ~(1 << btn.gpio_mask));
}

hid_gamepad_report_t get_report(booster_state_t *booster_left, booster_state_t *booster_right)
{
    hid_gamepad_report_t report = {0};

    if (!booster_left || !booster_right)
        return report;

    /* TODO: only update if booster state has changed */

    // Handle the left booster
    // Currenly this booster is forced to use the dpad
    update_button_mask(&report, booster_left->btn);
    report.hat = map_gamepad_hat(booster_left->up,
                                 booster_left->down,
                                 booster_left->left,
                                 booster_left->right);

    // Handle the right booster
    update_button_mask(&report, booster_right->btn);
    update_button_mask(&report, booster_right->up);
    update_button_mask(&report, booster_right->down);
    update_button_mask(&report, booster_right->left);
    update_button_mask(&report, booster_right->right);

    return report;
}
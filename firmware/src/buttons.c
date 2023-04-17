#include "buttons.h"
#include "hardware/gpio.h"
#include "pinmap.h"
#include "pico/time.h"

#define BUTTON_DEBOUNCE_MS 2

static button_t button_list[] = {
    {.gpio_pin = BOOSTER_RIGHT_UP, .gpio_mask = GAMEPAD_BUTTON_A, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_RIGHT_DOWN, .gpio_mask = GAMEPAD_BUTTON_B, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_RIGHT_LEFT, .gpio_mask = GAMEPAD_BUTTON_X, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_RIGHT_RIGHT, .gpio_mask = GAMEPAD_BUTTON_Y, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_RIGHT_BTN, .gpio_mask = GAMEPAD_BUTTON_TR, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_LEFT_BTN, .gpio_mask = GAMEPAD_BUTTON_TL, .last_change = 0, .active = false},
};
#define N_BUTTONS (sizeof(button_list) / sizeof(button_t))

static button_t dpad_button_list[] = {
    {.gpio_pin = BOOSTER_LEFT_UP, .gpio_mask = GAMEPAD_HAT_UP, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_LEFT_DOWN, .gpio_mask = GAMEPAD_HAT_DOWN, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_LEFT_LEFT, .gpio_mask = GAMEPAD_HAT_LEFT, .last_change = 0, .active = false},
    {.gpio_pin = BOOSTER_LEFT_RIGHT, .gpio_mask = GAMEPAD_HAT_RIGHT, .last_change = 0, .active = false},
};
#define N_DPAD_BUTTONS (sizeof(dpad_button_list) / sizeof(button_t))

// static dpad_t dpad = {
//     .up = {.gpio_pin = BOOSTER_LEFT_UP, .gpio_mask = 0, .last_change = 0, .active = false},
//     .down = {.gpio_pin = BOOSTER_LEFT_DOWN, .gpio_mask = 0, .last_change = 0, .active = false},
//     .left = {.gpio_pin = BOOSTER_LEFT_LEFT, .gpio_mask = 0, .last_change = 0, .active = false},
//     .right = {.gpio_pin = BOOSTER_LEFT_RIGHT, .gpio_mask = 0, .last_change = 0, .active = false},
// };

void init_button_gpio(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}

void init_buttons()
{
    // Init dpad
    // init_button_gpio(dpad.up.gpio_pin);
    // init_button_gpio(dpad.down.gpio_pin);
    // init_button_gpio(dpad.left.gpio_pin);
    // init_button_gpio(dpad.right.gpio_pin);
    for (size_t i = 0; i < N_DPAD_BUTTONS; i++)
    {
        button_t *btn = &dpad_button_list[i];
        init_button_gpio(btn->gpio_pin);
    }

    // Init buttons
    for (size_t i = 0; i < N_BUTTONS; i++)
    {
        button_t *btn = &button_list[i];
        init_button_gpio(btn->gpio_pin);
    }
}

static uint8_t get_gamepad_hat(bool up, bool down, bool left, bool right)
{
    switch ((up << 3) | (down << 2) | (left << 1) | right)
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

void update_dpad(hid_gamepad_report_t *report)
{
    uint32_t now = to_ms_since_boot(get_absolute_time());

    for (size_t i = 0; i < N_DPAD_BUTTONS; i++)
    {
        button_t *btn = &dpad_button_list[i];
        bool current_state = !gpio_get(btn->gpio_pin);

        // Only update if state has changed
        if (current_state == btn->active)
            continue;

        // Debounce by BUTTON_DEBOUNCE_MS to prevent mechanical jitter
        if (btn->last_change + BUTTON_DEBOUNCE_MS > now)
            continue;

        btn->active = current_state;
        btn->last_change = now;
    }

    report->hat = get_gamepad_hat(dpad_button_list[0].active,
                                  dpad_button_list[1].active,
                                  dpad_button_list[2].active,
                                  dpad_button_list[3].active);
}

void update_buttons(hid_gamepad_report_t *report)
{
    uint32_t now = to_ms_since_boot(get_absolute_time());

    for (size_t i = 0; i < N_BUTTONS; i++)
    {
        button_t *btn = &button_list[i];
        bool current_state = !gpio_get(btn->gpio_pin);

        // Only update if state has changed
        if (current_state == btn->active)
            continue;

        // Debounce by BUTTON_DEBOUNCE_MS to prevent mechanical jitter
        if (btn->last_change + BUTTON_DEBOUNCE_MS > now)
            continue;

        btn->active = current_state;
        btn->last_change = now;

        if (btn->active)
        {
            // Set the corresponding bit in the gamepad button mask
            report->buttons |= (1 << btn->gpio_mask);
        }
        else
        {
            // Clear the corresponding bit in the gamepad button mask
            report->buttons &= ~(1 << btn->gpio_mask);
        }
    }
}
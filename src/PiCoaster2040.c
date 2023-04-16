#include "PiCoaster2040.h"
#include "hardware/gpio.h"
#include "pinmap.h"
#include "tusb.h"
#include "usb/usb_descriptors.h"

void init_button_gpio(uint gpio);
static uint8_t get_gamepad_hat(bool up, bool down, bool left, bool right);

void setup()
{
    tud_init(BOARD_TUD_RHPORT);

    init_button_gpio(BOOSTER_LEFT_UP);
    init_button_gpio(BOOSTER_LEFT_DOWN);
    init_button_gpio(BOOSTER_LEFT_LEFT);
    init_button_gpio(BOOSTER_LEFT_RIGHT);
    init_button_gpio(BOOSTER_LEFT_BTN);

    init_button_gpio(BOOSTER_RIGHT_UP);
    init_button_gpio(BOOSTER_RIGHT_DOWN);
    init_button_gpio(BOOSTER_RIGHT_LEFT);
    init_button_gpio(BOOSTER_RIGHT_RIGHT);
    init_button_gpio(BOOSTER_RIGHT_BTN);
}

void update()
{
    tud_task();

    if (!tud_ready())
        return;

    uint32_t values = ~gpio_get_all();
    hid_gamepad_report_t report = {0};

    // Read the microswitch states from the input pins
    bool up = values & (1u << BOOSTER_LEFT_UP);
    bool down = values & (1u << BOOSTER_LEFT_DOWN);
    bool left = values & (1u << BOOSTER_LEFT_LEFT);
    bool right = values & (1u << BOOSTER_LEFT_RIGHT);

    // get the hat direction based on the microswitch states above
    report.hat = get_gamepad_hat(up, down, left, right);

    // Set gamepad button states based on the values of the input pins
    report.buttons = 0 | ((values & (1u << BOOSTER_RIGHT_UP)) ? GAMEPAD_BUTTON_A : 0) |
                     ((values & (1u << BOOSTER_RIGHT_DOWN)) ? GAMEPAD_BUTTON_B : 0) |
                     ((values & (1u << BOOSTER_RIGHT_LEFT)) ? GAMEPAD_BUTTON_X : 0) |
                     ((values & (1u << BOOSTER_RIGHT_RIGHT)) ? GAMEPAD_BUTTON_Y : 0) |
                     ((values & (1u << BOOSTER_RIGHT_BTN)) ? GAMEPAD_BUTTON_TR : 0) |
                     ((values & (1u << BOOSTER_LEFT_BTN)) ? GAMEPAD_BUTTON_TL : 0);

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

void init_button_gpio(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
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
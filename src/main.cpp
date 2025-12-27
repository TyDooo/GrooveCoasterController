#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <FastLED.h>

#include "booster.h"

#define NUM_LEDS 32
#define LEDS_PER_SEGMENT 4
#define NUM_SEGMENTS (NUM_LEDS / LEDS_PER_SEGMENT)

CRGB leds[NUM_LEDS];

const int leftPins[5] = {D12, D14, D15, D13, D11};
const int rightPins[5] = {D16, D17, D18, D19, D20};

Booster booster_left(leftPins);
Booster booster_right(rightPins);

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD()};

// USB HID object
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    FastLED.addLeds<NEOPIXEL, D21>(leds, NUM_LEDS);

    if (!TinyUSBDevice.isInitialized())
    {
        TinyUSBDevice.begin(0);
    }

    Serial.begin(115200);

    // Setup HID
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    // If already enumerated, additional class driver begin() e.g msc, hid, midi won't take effect until re-enumeration
    if (TinyUSBDevice.mounted())
    {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }
}

void set_segment_color(int segment, CRGB color)
{
    int start = segment * LEDS_PER_SEGMENT;
    int end = start + LEDS_PER_SEGMENT;
    for (int i = start; i < end; i++)
    {
        leds[i] = color;
        leds[i].subtractFromRGB(200);
    }
}

void send_gamepad_report()
{
    // not enumerated()/mounted() yet: nothing to do
    if (!TinyUSBDevice.mounted())
    {
        return;
    }

    if (!usb_hid.ready())
        return;

    // Reset gamepad report
    gp.x = 0;
    gp.y = 0;
    gp.z = 0;
    gp.rz = 0;
    gp.rx = 0;
    gp.ry = 0;
    gp.hat = 0;
    gp.buttons = 0;

    // Left booster

    switch (booster_left.getJoystickDirection())
    {
    case Booster::JoystickDirection::Up:
        gp.y = -127;
        break;
    case Booster::JoystickDirection::Down:
        gp.y = 127;
        break;
    case Booster::JoystickDirection::Left:
        gp.x = -127;
        break;
    case Booster::JoystickDirection::Right:
        gp.x = 127;
        break;
    case Booster::JoystickDirection::UpLeft:
        gp.y = -127;
        gp.x = -127;
        break;
    case Booster::JoystickDirection::UpRight:
        gp.y = -127;
        gp.x = 127;
        break;
    case Booster::JoystickDirection::DownLeft:
        gp.y = 127;
        gp.x = -127;
        break;
    case Booster::JoystickDirection::DownRight:
        gp.y = 127;
        gp.x = 127;
        break;
    default:
        break;
    }

    if (booster_left.isButtonPressed(Booster::ButtonType::Top))
    {
        gp.buttons |= GAMEPAD_BUTTON_0;
    }

    // Right booster

    switch (booster_right.getJoystickDirection())
    {
    case Booster::JoystickDirection::Up:
        gp.ry = -127;
        break;
    case Booster::JoystickDirection::Down:
        gp.ry = 127;
        break;
    case Booster::JoystickDirection::Left:
        gp.rx = -127;
        break;
    case Booster::JoystickDirection::Right:
        gp.rx = 127;
        break;
    case Booster::JoystickDirection::UpLeft:
        gp.ry = -127;
        gp.rx = -127;
        break;
    case Booster::JoystickDirection::UpRight:
        gp.ry = -127;
        gp.rx = 127;
        break;
    case Booster::JoystickDirection::DownLeft:
        gp.ry = 127;
        gp.rx = -127;
        break;
    case Booster::JoystickDirection::DownRight:
        gp.ry = 127;
        gp.rx = 127;
        break;
    default:
        break;
    }

    if (booster_right.isButtonPressed(Booster::ButtonType::Top))
    {
        gp.buttons |= GAMEPAD_BUTTON_1;
    }

    usb_hid.sendReport(0, &gp, sizeof(gp));
}

void loop()
{
#ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
#endif

    booster_left.update();
    booster_right.update();

    // Reset LEDs
    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        set_segment_color(i, CRGB::White);
    }

    // Left booster

    switch (booster_left.getJoystickDirection())
    {
    case Booster::JoystickDirection::Up:
        set_segment_color(1, CRGB::Red);
        set_segment_color(2, CRGB::Red);
        break;
    case Booster::JoystickDirection::Down:
        set_segment_color(6, CRGB::Red);
        set_segment_color(5, CRGB::Red);
        break;
    case Booster::JoystickDirection::Left:
        set_segment_color(0, CRGB::Red);
        set_segment_color(7, CRGB::Red);
        break;
    case Booster::JoystickDirection::Right:
        set_segment_color(3, CRGB::Red);
        set_segment_color(4, CRGB::Red);
        break;
    case Booster::JoystickDirection::UpLeft:
        set_segment_color(0, CRGB::Red);
        break;
    case Booster::JoystickDirection::UpRight:
        set_segment_color(2, CRGB::Red);
        break;
    case Booster::JoystickDirection::DownLeft:
        set_segment_color(7, CRGB::Red);
        break;
    case Booster::JoystickDirection::DownRight:
        set_segment_color(5, CRGB::Red);
        break;
    default:
        break;
    }

    FastLED.show();

    send_gamepad_report();

    delay(50);
}

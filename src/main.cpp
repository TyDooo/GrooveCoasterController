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

void loop()
{
#ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
#endif

    booster_left.update();
    booster_right.update();

    // Reset gamepad report
    gp.x = 0;
    gp.y = 0;
    gp.z = 0;
    gp.rz = 0;
    gp.rx = 0;
    gp.ry = 0;
    gp.hat = 0;
    gp.buttons = 0;

    // Reset LEDs
    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        set_segment_color(i, CRGB::White);
    }

    // Left booster

    if (booster_left.isButtonPressed(Booster::ButtonType::Up) && booster_left.isButtonPressed(Booster::ButtonType::Left))
    {
        gp.y = -127;
        gp.x = -127;
        set_segment_color(0, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Up) && booster_left.isButtonPressed(Booster::ButtonType::Right))
    {
        gp.y = -127;
        gp.x = 127;
        set_segment_color(2, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Down) && booster_left.isButtonPressed(Booster::ButtonType::Left))
    {
        gp.y = 127;
        gp.x = -127;
        set_segment_color(7, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Down) && booster_left.isButtonPressed(Booster::ButtonType::Right))
    {
        gp.y = 127;
        gp.x = 127;
        set_segment_color(5, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Up))
    {
        gp.y = -127;
        set_segment_color(1, CRGB::Red);
        set_segment_color(2, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Down))
    {
        gp.y = 127;
        set_segment_color(6, CRGB::Red);
        set_segment_color(5, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Left))
    {
        gp.x = -127;
        set_segment_color(0, CRGB::Red);
        set_segment_color(7, CRGB::Red);
    }
    else if (booster_left.isButtonPressed(Booster::ButtonType::Right))
    {
        gp.x = 127;
        set_segment_color(3, CRGB::Red);
        set_segment_color(4, CRGB::Red);
    }

    if (booster_left.isButtonPressed(Booster::ButtonType::Top))
    {
        gp.buttons |= GAMEPAD_BUTTON_0;
    }

    // Right booster

    if (booster_right.isButtonPressed(Booster::ButtonType::Up))
    {
        gp.ry = -127;
    }
    else if (booster_right.isButtonPressed(Booster::ButtonType::Down))
    {
        gp.ry = 127;
    }

    if (booster_right.isButtonPressed(Booster::ButtonType::Left))
    {
        gp.rx = -127;
    }
    else if (booster_right.isButtonPressed(Booster::ButtonType::Right))
    {
        gp.rx = 127;
    }

    if (booster_right.isButtonPressed(Booster::ButtonType::Top))
    {
        gp.buttons |= GAMEPAD_BUTTON_1;
    }

    FastLED.show();

    // not enumerated()/mounted() yet: nothing to do
    if (!TinyUSBDevice.mounted())
    {
        return;
    }

    if (!usb_hid.ready())
        return;

    usb_hid.sendReport(0, &gp, sizeof(gp));

    delay(50);
}

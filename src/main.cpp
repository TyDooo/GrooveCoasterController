#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <FastLED.h>

#include "booster.h"

#define NUM_LEDS 32
#define LEDS_PER_SEGMENT 4
#define NUM_SEGMENTS (NUM_LEDS / LEDS_PER_SEGMENT)

// Gamepad axis constants
#define AXIS_MIN -127
#define AXIS_MAX 127
#define AXIS_CENTER 0

CRGB leds[NUM_LEDS];

const int leftPins[5] = {D12, D14, D15, D13, D11};
const int rightPins[5] = {D16, D17, D18, D19, D20};

Booster booster_left(leftPins);
Booster booster_right(rightPins);

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_GAMEPAD()};

// USB HID object
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

// Helper function to map joystick direction to axis values
void mapDirectionToAxes(Booster::JoystickDirection direction, int8_t& xAxis,
                        int8_t& yAxis)
{
  xAxis = AXIS_CENTER;
  yAxis = AXIS_CENTER;

  switch (direction) {
    case Booster::JoystickDirection::Up:
      yAxis = AXIS_MIN;
      break;
    case Booster::JoystickDirection::Down:
      yAxis = AXIS_MAX;
      break;
    case Booster::JoystickDirection::Left:
      xAxis = AXIS_MIN;
      break;
    case Booster::JoystickDirection::Right:
      xAxis = AXIS_MAX;
      break;
    case Booster::JoystickDirection::UpLeft:
      yAxis = AXIS_MIN;
      xAxis = AXIS_MIN;
      break;
    case Booster::JoystickDirection::UpRight:
      yAxis = AXIS_MIN;
      xAxis = AXIS_MAX;
      break;
    case Booster::JoystickDirection::DownLeft:
      yAxis = AXIS_MAX;
      xAxis = AXIS_MIN;
      break;
    case Booster::JoystickDirection::DownRight:
      yAxis = AXIS_MAX;
      xAxis = AXIS_MAX;
      break;
    default:
      break;
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  FastLED.addLeds<NEOPIXEL, D21>(leds, NUM_LEDS);

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  // Setup HID
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // If already enumerated, additional class driver begin() e.g msc, hid, midi
  // won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void set_segment_color(int segment, CRGB color)
{
  int start = segment * LEDS_PER_SEGMENT;
  int end = start + LEDS_PER_SEGMENT;
  for (int i = start; i < end; i++) {
    leds[i] = color;
    leds[i].subtractFromRGB(200);
  }
}

void send_gamepad_report()
{
  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  if (!usb_hid.ready()) return;

  // Reset gamepad report
  gp.x = AXIS_CENTER;
  gp.y = AXIS_CENTER;
  gp.z = AXIS_CENTER;
  gp.rz = AXIS_CENTER;
  gp.rx = AXIS_CENTER;
  gp.ry = AXIS_CENTER;
  gp.hat = GAMEPAD_HAT_CENTERED;
  gp.buttons = 0;

  // Map left booster to left stick (X, Y axes)
  mapDirectionToAxes(booster_left.getJoystickDirection(), gp.x, gp.y);

  // Map left booster top button to gamepad button
  if (booster_left.isTopButtonPressed()) gp.buttons |= GAMEPAD_BUTTON_EAST;

  // Map right booster to right stick (RX, RY axes)
  mapDirectionToAxes(booster_right.getJoystickDirection(), gp.rx, gp.ry);

  // Map right booster top button to gamepad button
  if (booster_right.isTopButtonPressed()) gp.buttons |= GAMEPAD_BUTTON_SOUTH;

  usb_hid.sendReport(0, &gp, sizeof(gp));
}

void update_leds()
{
  static uint8_t brightness = 20;
  static int8_t direction = 2;

  brightness += direction;
  if (brightness <= 20 || brightness >= 200) {
    direction = -direction;
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(0, 0, brightness);
  }

  // Left booster

  switch (booster_left.getJoystickDirection()) {
    case Booster::JoystickDirection::Up:
      set_segment_color(0, CRGB::Red);
      set_segment_color(1, CRGB::Red);
      set_segment_color(2, CRGB::Red);
      set_segment_color(3, CRGB::Red);
      break;
    case Booster::JoystickDirection::Down:
      set_segment_color(7, CRGB::Red);
      set_segment_color(6, CRGB::Red);
      set_segment_color(5, CRGB::Red);
      set_segment_color(4, CRGB::Red);
      break;
    case Booster::JoystickDirection::Left:
      set_segment_color(0, CRGB::Red);
      set_segment_color(1, CRGB::Red);
      set_segment_color(6, CRGB::Red);
      set_segment_color(7, CRGB::Red);
      break;
    case Booster::JoystickDirection::Right:
      set_segment_color(2, CRGB::Red);
      set_segment_color(3, CRGB::Red);
      set_segment_color(4, CRGB::Red);
      set_segment_color(5, CRGB::Red);
      break;
    case Booster::JoystickDirection::UpLeft:
      set_segment_color(0, CRGB::Red);
      set_segment_color(1, CRGB::Red);
      break;
    case Booster::JoystickDirection::UpRight:
      set_segment_color(2, CRGB::Red);
      set_segment_color(3, CRGB::Red);
      break;
    case Booster::JoystickDirection::DownLeft:
      set_segment_color(6, CRGB::Red);
      set_segment_color(7, CRGB::Red);
      break;
    case Booster::JoystickDirection::DownRight:
      set_segment_color(4, CRGB::Red);
      set_segment_color(5, CRGB::Red);
      break;
    default:
      break;
  }

  FastLED.show();
}

void loop()
{
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  booster_left.update();
  booster_right.update();

  EVERY_N_MILLISECONDS(20) { update_leds(); }

  send_gamepad_report();
}

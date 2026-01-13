#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <FastLED.h>

#include "booster.h"

// Gamepad axis constants
#define AXIS_MIN -127
#define AXIS_MAX 127
#define AXIS_CENTER 0

CRGB booster_left_leds[NUM_LEDS];
CRGB booster_right_leds[NUM_LEDS];

const int leftPins[5] = {D9, D8, D7, D6, D5};
const int rightPins[5] = {D18, D19, D20, D21, D22};

Booster booster_left(leftPins, booster_left_leds);
Booster booster_right(rightPins, booster_right_leds);

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

  FastLED.addLeds<NEOPIXEL, D15>(booster_left_leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, D16>(booster_right_leds, NUM_LEDS);

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

  // The joystick directions are mapped to the hat and A/B/X/Y buttons and the
  // top buttons are mapped to the TL/TR buttons. This is done to allow for
  // proper(ish) controller movement in the menu of the game and perform most
  // actions. Mapping the top buttons to A and B is not possible as it would
  // prevent you from hitting double presses in the game. This mapping has been
  // tested in Groove Coaster wai wai party and works fine for now.

  // Map left booster to left stick (X, Y axes)
  // NOTE: For the switch, NORTH and WEST are swapped compared to standard
  // gamepad mapping.

  switch (booster_left.getJoystickDirection()) {
    case Booster::JoystickDirection::Up:
      gp.buttons |= GAMEPAD_BUTTON_WEST;
      break;
    case Booster::JoystickDirection::Down:
      gp.buttons |= GAMEPAD_BUTTON_SOUTH;
      break;
    case Booster::JoystickDirection::Left:
      gp.buttons |= GAMEPAD_BUTTON_NORTH;
      break;
    case Booster::JoystickDirection::Right:
      gp.buttons |= GAMEPAD_BUTTON_EAST;
      break;
    case Booster::JoystickDirection::UpLeft:
      gp.buttons |= GAMEPAD_BUTTON_NORTH | GAMEPAD_BUTTON_WEST;
      break;
    case Booster::JoystickDirection::UpRight:
      gp.buttons |= GAMEPAD_BUTTON_WEST | GAMEPAD_BUTTON_EAST;
      break;
    case Booster::JoystickDirection::DownLeft:
      gp.buttons |= GAMEPAD_BUTTON_SOUTH | GAMEPAD_BUTTON_NORTH;
      break;
    case Booster::JoystickDirection::DownRight:
      gp.buttons |= GAMEPAD_BUTTON_SOUTH | GAMEPAD_BUTTON_EAST;
      break;
    default:
      break;
  }

  // Map left booster top button to gamepad button
  if (booster_left.isTopButtonPressed()) gp.buttons |= GAMEPAD_BUTTON_TL;

  // Map right booster to right stick (RX, RY axes)
  // mapDirectionToAxes(booster_right.getJoystickDirection(), gp.rx, gp.ry);

  switch (booster_right.getJoystickDirection()) {
    case Booster::JoystickDirection::Up:
      gp.hat = GAMEPAD_HAT_UP;
      break;
    case Booster::JoystickDirection::Down:
      gp.hat = GAMEPAD_HAT_DOWN;
      break;
    case Booster::JoystickDirection::Left:
      gp.hat = GAMEPAD_HAT_LEFT;
      break;
    case Booster::JoystickDirection::Right:
      gp.hat = GAMEPAD_HAT_RIGHT;
      break;
    case Booster::JoystickDirection::UpLeft:
      gp.hat = GAMEPAD_HAT_UP_LEFT;
      break;
    case Booster::JoystickDirection::UpRight:
      gp.hat = GAMEPAD_HAT_UP_RIGHT;
      break;
    case Booster::JoystickDirection::DownLeft:
      gp.hat = GAMEPAD_HAT_DOWN_LEFT;
      break;
    case Booster::JoystickDirection::DownRight:
      gp.hat = GAMEPAD_HAT_DOWN_RIGHT;
      break;
    default:
      gp.hat = GAMEPAD_HAT_CENTERED;
      break;
  }

  // Map right booster top button to gamepad button
  if (booster_right.isTopButtonPressed()) gp.buttons |= GAMEPAD_BUTTON_TR;

  usb_hid.sendReport(0, &gp, sizeof(gp));
}

CRGB ripple_leds_left[NUM_LEDS];
CRGB ripple_leds_right[NUM_LEDS];

#define MAX_RIPPLES 5

struct RippleEffect {
  bool active = false;
  int center1 = 0;
  int center2 = 1;
  float pos = -1.0;
};

RippleEffect leftRipples[MAX_RIPPLES];
RippleEffect rightRipples[MAX_RIPPLES];

void ripple(CRGB* leds, RippleEffect* ripples)
{
  const float speed = 0.4;

  fadeToBlackBy(leds, NUM_LEDS, 100);

  uint8_t maxDist = NUM_SEGMENTS / 2;

  for (int r = 0; r < MAX_RIPPLES; r++) {
    if (!ripples[r].active) continue;

    ripples[r].pos += speed;

    for (int i = 0; i < NUM_SEGMENTS; i++) {
      int d1 = abs(i - ripples[r].center1);
      if (d1 > maxDist) d1 = NUM_SEGMENTS - d1;  // Wrap around
      int d2 = abs(i - ripples[r].center2);
      if (d2 > maxDist) d2 = NUM_SEGMENTS - d2;  // Wrap around
      uint8_t distance = min(d1, d2);

      if (abs(ripples[r].pos - distance) < 0.8) {
        fill_solid(&leds[i * LEDS_PER_SEGMENT], LEDS_PER_SEGMENT, CRGB::Purple);
      }
    }

    if (ripples[r].pos > maxDist + 1) {
      ripples[r].active = false;
    }
  }
}

void trigger_ripple(RippleEffect* ripples, int center1, int center2)
{
  for (int i = 0; i < MAX_RIPPLES; i++) {
    if (!ripples[i].active) {
      ripples[i].pos = 0.0;
      ripples[i].active = true;
      ripples[i].center1 = center1;
      ripples[i].center2 = center2;
      break;
    }
  }
}

void update_leds()
{
  const int BPM = 100;
  const int MIN_BRIGHTNESS = 10;
  const int MAX_BRIGHTNESS = 30;

  // Background breathing effect
  uint8_t brightness = beatsin8(BPM, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  CRGB bg_color = CRGB::White;
  bg_color.fadeToBlackBy(255 - brightness);
  fill_solid(booster_left_leds, NUM_LEDS, bg_color);
  fill_solid(booster_right_leds, NUM_LEDS, bg_color);

  ripple(ripple_leds_left, leftRipples);
  ripple(ripple_leds_right, rightRipples);

  // Overlay ripple effect
  for (int i = 0; i < NUM_LEDS; i++) {
    booster_left_leds[i] =
        CRGB::blend(booster_left_leds[i], ripple_leds_left[i], 200);
    booster_right_leds[i] =
        CRGB::blend(booster_right_leds[i], ripple_leds_right[i], 200);
  }

  // Left booster

  static Booster::JoystickDirection lastDirectionLeft =
      Booster::JoystickDirection::Center;

  if (lastDirectionLeft != booster_left.getJoystickDirection()) {
    switch (lastDirectionLeft) {
      case Booster::JoystickDirection::Up:
        trigger_ripple(leftRipples, 5, 6);
        break;
      case Booster::JoystickDirection::Down:
        trigger_ripple(leftRipples, 1, 2);
        break;
      case Booster::JoystickDirection::Left:
        trigger_ripple(leftRipples, 3, 4);
        break;
      case Booster::JoystickDirection::Right:
        trigger_ripple(leftRipples, 0, 7);
        break;
      case Booster::JoystickDirection::UpLeft:
        trigger_ripple(leftRipples, 4, 5);
        break;
      case Booster::JoystickDirection::UpRight:
        trigger_ripple(leftRipples, 6, 7);
        break;
      case Booster::JoystickDirection::DownLeft:
        trigger_ripple(leftRipples, 2, 3);
        break;
      case Booster::JoystickDirection::DownRight:
        trigger_ripple(leftRipples, 0, 1);
        break;
      default:
        break;
    }
  }

  lastDirectionLeft = booster_left.getJoystickDirection();

  // Right booster

  static Booster::JoystickDirection lastDirectionRight =
      Booster::JoystickDirection::Center;

  if (lastDirectionRight != booster_right.getJoystickDirection()) {
    switch (lastDirectionRight) {
      case Booster::JoystickDirection::Up:
        trigger_ripple(rightRipples, 1, 2);
        break;
      case Booster::JoystickDirection::Down:
        trigger_ripple(rightRipples, 5, 6);
        break;
      case Booster::JoystickDirection::Left:
        trigger_ripple(rightRipples, 0, 7);
        break;
      case Booster::JoystickDirection::Right:
        trigger_ripple(rightRipples, 3, 4);
        break;
      case Booster::JoystickDirection::UpLeft:
        trigger_ripple(rightRipples, 0, 1);
        break;
      case Booster::JoystickDirection::UpRight:
        trigger_ripple(rightRipples, 2, 3);
        break;
      case Booster::JoystickDirection::DownLeft:
        trigger_ripple(rightRipples, 6, 7);
        break;
      case Booster::JoystickDirection::DownRight:
        trigger_ripple(rightRipples, 4, 5);
        break;
      default:
        break;
    }
  }

  lastDirectionRight = booster_right.getJoystickDirection();

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

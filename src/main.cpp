#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <FastLED.h>

#include "booster.h"
#include "effects.h"

// Gamepad axis constants
#define AXIS_MIN -127
#define AXIS_MAX 127
#define AXIS_CENTER 0

CRGB booster_left_leds[NUM_LEDS];
CRGB booster_right_leds[NUM_LEDS];

EffectManager booster_left_effect_manager;
EffectManager booster_right_effect_manager;

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

void update_leds()
{
  fill_solid(booster_left_leds, NUM_LEDS, CHSV(0, 0, 50));
  fill_solid(booster_right_leds, NUM_LEDS, CHSV(0, 0, 50));

  booster_left_effect_manager.updateEffects(booster_left_leds);
  booster_right_effect_manager.updateEffects(booster_right_leds);

  // Left booster

  static Booster::JoystickDirection lastDirectionLeft =
      Booster::JoystickDirection::Center;

  if (lastDirectionLeft != booster_left.getJoystickDirection()) {
    switch (lastDirectionLeft) {
      case Booster::JoystickDirection::Up:
        booster_left_effect_manager.startEffect(new RippleEffect{5, 6});
        break;
      case Booster::JoystickDirection::Down:
        booster_left_effect_manager.startEffect(new RippleEffect{1, 2});
        break;
      case Booster::JoystickDirection::Left:
        booster_left_effect_manager.startEffect(new RippleEffect{3, 4});
        break;
      case Booster::JoystickDirection::Right:
        booster_left_effect_manager.startEffect(new RippleEffect{0, 7});
        break;
      case Booster::JoystickDirection::UpLeft:
        booster_left_effect_manager.startEffect(new RippleEffect{4, 5});
        break;
      case Booster::JoystickDirection::UpRight:
        booster_left_effect_manager.startEffect(new RippleEffect{6, 7});
        break;
      case Booster::JoystickDirection::DownLeft:
        booster_left_effect_manager.startEffect(new RippleEffect{2, 3});
        break;
      case Booster::JoystickDirection::DownRight:
        booster_left_effect_manager.startEffect(new RippleEffect{0, 1});
        break;
      default:
        break;
    }
  }

  static bool lastTopButtonLeft = false;
  if (lastTopButtonLeft != booster_left.isTopButtonPressed() &&
      lastTopButtonLeft) {
    booster_left_effect_manager.startEffect(new FadeInOutEffect(200));
  }
  lastTopButtonLeft = booster_left.isTopButtonPressed();

  lastDirectionLeft = booster_left.getJoystickDirection();

  // Right booster

  static Booster::JoystickDirection lastDirectionRight =
      Booster::JoystickDirection::Center;

  if (lastDirectionRight != booster_right.getJoystickDirection()) {
    switch (lastDirectionRight) {
      case Booster::JoystickDirection::Up:
        booster_right_effect_manager.startEffect(new RippleEffect{1, 2});
        break;
      case Booster::JoystickDirection::Down:
        booster_right_effect_manager.startEffect(new RippleEffect{5, 6});
        break;
      case Booster::JoystickDirection::Left:
        booster_right_effect_manager.startEffect(new RippleEffect{0, 7});
        break;
      case Booster::JoystickDirection::Right:
        booster_right_effect_manager.startEffect(new RippleEffect{3, 4});
        break;
      case Booster::JoystickDirection::UpLeft:
        booster_right_effect_manager.startEffect(new RippleEffect{0, 1});
        break;
      case Booster::JoystickDirection::UpRight:
        booster_right_effect_manager.startEffect(new RippleEffect{2, 3});
        break;
      case Booster::JoystickDirection::DownLeft:
        booster_right_effect_manager.startEffect(new RippleEffect{6, 7});
        break;
      case Booster::JoystickDirection::DownRight:
        booster_right_effect_manager.startEffect(new RippleEffect{4, 5});
        break;
      default:
        break;
    }
  }

  lastDirectionRight = booster_right.getJoystickDirection();

  static bool lastTopButtonRight = false;
  if (lastTopButtonRight != booster_right.isTopButtonPressed() &&
      lastTopButtonRight) {
    booster_right_effect_manager.startEffect(new FadeInOutEffect(200));
  }
  lastTopButtonRight = booster_right.isTopButtonPressed();

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

  EVERY_N_MILLISECONDS(LED_UPDATE_INTERVAL_MS) { update_leds(); }

  send_gamepad_report();
}

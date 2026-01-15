#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <FastLED.h>

#include "boosterinput.h"
#include "ledmanager.h"

static const int leftPins[5] = {D9, D8, D7, D6, D5};
static const int rightPins[5] = {D18, D19, D20, D21, D22};

BoosterInput booster_left(leftPins);
BoosterInput booster_right(rightPins);

LEDManager ledManager(&booster_left, &booster_right);

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_GAMEPAD()};

// USB HID object
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  // Setup HID
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  ledManager.setup();

  // If already enumerated, additional class driver begin() e.g msc, hid, midi
  // won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void sendGamepadReport()
{
  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  if (!usb_hid.ready()) return;

  // Reset gamepad report
  gp.x = 0;
  gp.y = 0;
  gp.z = 0;
  gp.rz = 0;
  gp.rx = 0;
  gp.ry = 0;
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
    case BoosterInput::JoystickDirection::Up:
      gp.buttons |= GAMEPAD_BUTTON_WEST;
      break;
    case BoosterInput::JoystickDirection::Down:
      gp.buttons |= GAMEPAD_BUTTON_SOUTH;
      break;
    case BoosterInput::JoystickDirection::Left:
      gp.buttons |= GAMEPAD_BUTTON_NORTH;
      break;
    case BoosterInput::JoystickDirection::Right:
      gp.buttons |= GAMEPAD_BUTTON_EAST;
      break;
    case BoosterInput::JoystickDirection::UpLeft:
      gp.buttons |= GAMEPAD_BUTTON_NORTH | GAMEPAD_BUTTON_WEST;
      break;
    case BoosterInput::JoystickDirection::UpRight:
      gp.buttons |= GAMEPAD_BUTTON_WEST | GAMEPAD_BUTTON_EAST;
      break;
    case BoosterInput::JoystickDirection::DownLeft:
      gp.buttons |= GAMEPAD_BUTTON_SOUTH | GAMEPAD_BUTTON_NORTH;
      break;
    case BoosterInput::JoystickDirection::DownRight:
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
    case BoosterInput::JoystickDirection::Up:
      gp.hat = GAMEPAD_HAT_UP;
      break;
    case BoosterInput::JoystickDirection::Down:
      gp.hat = GAMEPAD_HAT_DOWN;
      break;
    case BoosterInput::JoystickDirection::Left:
      gp.hat = GAMEPAD_HAT_LEFT;
      break;
    case BoosterInput::JoystickDirection::Right:
      gp.hat = GAMEPAD_HAT_RIGHT;
      break;
    case BoosterInput::JoystickDirection::UpLeft:
      gp.hat = GAMEPAD_HAT_UP_LEFT;
      break;
    case BoosterInput::JoystickDirection::UpRight:
      gp.hat = GAMEPAD_HAT_UP_RIGHT;
      break;
    case BoosterInput::JoystickDirection::DownLeft:
      gp.hat = GAMEPAD_HAT_DOWN_LEFT;
      break;
    case BoosterInput::JoystickDirection::DownRight:
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

void loop()
{
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  booster_left.update();
  booster_right.update();

  ledManager.update();

  sendGamepadReport();
}

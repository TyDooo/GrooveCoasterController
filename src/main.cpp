#define GAMEPAD_DEBOUNCE_MILLIS 5

#include <Arduino.h>
#include "Gamepad.h"
#include "usb_driver.h"

Gamepad gamepad(GAMEPAD_DEBOUNCE_MILLIS);

void setup()
{
  gamepad.setup();

  pinMode(LED_BUILTIN, OUTPUT);

  // For now, force the input mode to INPUT_MODE_SWITCH
  // In the future this should be selectable
  gamepad.options.inputMode = INPUT_MODE_SWITCH;

  initialize_driver(gamepad.options.inputMode);
}

void loop()
{
  gamepad.read();
#if GAMEPAD_DEBOUNCE_MILLIS > 0
  gamepad.debounce();
#endif
  gamepad.process();

  send_report(gamepad.getReport(), gamepad.getReportSize());

  usb_task();
}

#include <Arduino.h>
#include <Joystick.h>
// #define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include "pinmap.h"

#define DEBOUNCE_MILLIS 5

struct Button
{
  uint8_t pin;
  Bounce bounce;
  bool state;
};

Button buttonMap[NUM_BTN] = {
    {.pin = BOOSTER_L_U},
    {.pin = BOOSTER_L_R},
    {.pin = BOOSTER_L_D},
    {.pin = BOOSTER_L_L},
    {.pin = BOOSTER_L_BTN},
    {.pin = BOOSTER_R_U},
    {.pin = BOOSTER_R_R},
    {.pin = BOOSTER_R_D},
    {.pin = BOOSTER_R_L},
    {.pin = BOOSTER_R_BTN}};

// Create the Joystick
Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
    2, 0,
    true, true, false,
    true, true, false,
    false, false,
    false, false, false);

void setup()
{
  // Button setup
  for (size_t i = 0; i < NUM_BTN; i++)
  {
    buttonMap[i].bounce = Bounce();
    buttonMap[i].bounce.attach(buttonMap[i].pin, INPUT_PULLUP);
    buttonMap[i].bounce.interval(DEBOUNCE_MILLIS);
    buttonMap[i].state = false;
  }

  // Initialize the Joystick library
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
  Joystick.setRxAxisRange(-1, 1);
  Joystick.setRyAxisRange(-1, 1);
}

void loop()
{
  for (size_t i = 0; i < NUM_BTN; i++)
  {
    if (!buttonMap[i].bounce.update())
      continue;

    bool currentState = buttonMap[i].bounce.fell();
    if (buttonMap[i].state != currentState)
    {
      switch (buttonMap[i].pin)
      {
      case BOOSTER_L_U:
        if (currentState)
        {
          Joystick.setYAxis(-1);
        }
        else
        {
          Joystick.setYAxis(0);
        }
        break;
      case BOOSTER_L_R:
        if (currentState)
        {
          Joystick.setXAxis(1);
        }
        else
        {
          Joystick.setXAxis(0);
        }
        break;
      case BOOSTER_L_D:
        if (currentState)
        {
          Joystick.setYAxis(1);
        }
        else
        {
          Joystick.setYAxis(0);
        }
        break;
      case BOOSTER_L_L:
        if (currentState)
        {
          Joystick.setXAxis(-1);
        }
        else
        {
          Joystick.setXAxis(0);
        }
        break;
      case BOOSTER_L_BTN:
        Joystick.setButton(0, currentState);
        break;
      case BOOSTER_R_U:
        if (currentState)
        {
          Joystick.setRyAxis(-1);
        }
        else
        {
          Joystick.setRyAxis(0);
        }
        break;
      case BOOSTER_R_R:
        if (currentState)
        {
          Joystick.setRxAxis(1);
        }
        else
        {
          Joystick.setRxAxis(0);
        }
        break;
      case BOOSTER_R_D:
        if (currentState)
        {
          Joystick.setRyAxis(1);
        }
        else
        {
          Joystick.setRyAxis(0);
        }
        break;
      case BOOSTER_R_L:
        if (currentState)
        {
          Joystick.setRxAxis(-1);
        }
        else
        {
          Joystick.setRxAxis(0);
        }
        break;
      case BOOSTER_R_BTN:
        Joystick.setButton(1, currentState);
        break;
      }
    }
    buttonMap[i].state = currentState;
  }
}
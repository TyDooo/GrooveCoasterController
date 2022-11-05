#include <Arduino.h>
#include <Joystick.h>

#define N_BUTTONS 10

int lastButtonState[N_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buttonMap[N_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 16};

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
  // Initialize booster left pins
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);

  // Initialize booster right pins
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);

  // Initialize the Joystick library
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
  Joystick.setRxAxisRange(-1, 1);
  Joystick.setRyAxisRange(-1, 1);
}

void loop()
{
  for (size_t i = 0; i < N_BUTTONS; i++)
  {
    int currentButtonState = !digitalRead(buttonMap[i]);
    if (currentButtonState != lastButtonState[i])
    {
      switch (i)
      {
      case 0: // BOOSTER_L UP
        if (currentButtonState == 1)
        {
          Joystick.setYAxis(-1);
        }
        else
        {
          Joystick.setYAxis(0);
        }
        break;
      case 1: // BOOSTER_L RIGHT
        if (currentButtonState == 1)
        {
          Joystick.setXAxis(1);
        }
        else
        {
          Joystick.setXAxis(0);
        }
        break;
      case 2: // BOOSTER_L DOWN
        if (currentButtonState == 1)
        {
          Joystick.setYAxis(1);
        }
        else
        {
          Joystick.setYAxis(0);
        }
        break;
      case 3: // BOOSTER_L LEFT
        if (currentButtonState == 1)
        {
          Joystick.setXAxis(-1);
        }
        else
        {
          Joystick.setXAxis(0);
        }
        break;
      case 4: // BOOSTER_R UP
        if (currentButtonState == 1)
        {
          Joystick.setRyAxis(-1);
        }
        else
        {
          Joystick.setRyAxis(0);
        }
        break;
      case 5: // BOOSTER_R RIGHT
        if (currentButtonState == 1)
        {
          Joystick.setRxAxis(1);
        }
        else
        {
          Joystick.setRxAxis(0);
        }
        break;
      case 6: // BOOSTER_R DOWN
        if (currentButtonState == 1)
        {
          Joystick.setRyAxis(1);
        }
        else
        {
          Joystick.setRyAxis(0);
        }
        break;
      case 7: // BOOSTER_R LEFT
        if (currentButtonState == 1)
        {
          Joystick.setRxAxis(-1);
        }
        else
        {
          Joystick.setRxAxis(0);
        }
        break;
      case 8: // BOOSTER_L BTN
        Joystick.setButton(0, currentButtonState);
        break;
      case 9: // BOOSTER_R BTN
        Joystick.setButton(1, currentButtonState);
        break;
      }
      lastButtonState[i] = currentButtonState;
    }
  }

  delay(10);
}
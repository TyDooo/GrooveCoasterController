#pragma once

#include <Bounce2.h>

#define BOOSTER_BUTTON_COUNT 5

class BoosterInput
{
 public:
  enum class ButtonType { Up, Down, Left, Right, Top };

  enum class JoystickDirection {
    Center,
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    DownRight
  };

  BoosterInput(const int buttonPins[BOOSTER_BUTTON_COUNT]);
  bool isTopButtonPressed() const;
  JoystickDirection getJoystickDirection() const { return direction_; }
  void update();

 private:
  Bounce2::Button buttons_[BOOSTER_BUTTON_COUNT];
  JoystickDirection direction_;
  unsigned long lastDiagonalTime_ = 0;

  bool isButtonPressed(ButtonType button) const;
};

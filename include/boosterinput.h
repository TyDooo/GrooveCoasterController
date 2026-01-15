#pragma once

#include <Bounce2.h>
#include <pico/mutex.h>

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

  // Thread-safe methods (use mutex)
  bool isTopButtonPressed() const;
  JoystickDirection getJoystickDirection() const;

  // Unsafe methods (no mutex)
  bool isTopButtonPressedUnsafe() const
  {
    return buttons_[(int)ButtonType::Top].isPressed();
  }
  JoystickDirection getJoystickDirectionUnsafe() const { return direction_; }

  void update();

 private:
  Bounce2::Button buttons_[BOOSTER_BUTTON_COUNT];
  JoystickDirection direction_;
  unsigned long lastDiagonalTime_ = 0;
  mutable mutex_t mutex_;

  bool isButtonPressed(ButtonType button) const;
};

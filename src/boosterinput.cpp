#include "boosterinput.h"

#include <Arduino.h>

// Keep these zero for now, I've not ran into any issues yet. If anything,
// it has gotten more responsive with debouncing disabled. Enable if needed.
#define BUTTON_DEBOUNCE_MS 0
#define DIAGONAL_WINDOW_MS 0  // Does wai wai party even use diagonals...?

BoosterInput::BoosterInput(const int buttonPins[BOOSTER_BUTTON_COUNT])
    : direction_(JoystickDirection::Center)
{
  mutex_init(&mutex_);
  
  for (int i = 0; i < BOOSTER_BUTTON_COUNT; ++i) {
    buttons_[i] = Bounce2::Button();
    buttons_[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons_[i].interval(BUTTON_DEBOUNCE_MS);
    buttons_[i].setPressedState(LOW);
  }
}

bool BoosterInput::isButtonPressed(ButtonType button) const
{
  mutex_enter_blocking(&mutex_);
  bool pressed = buttons_[(int)button].isPressed();
  mutex_exit(&mutex_);
  return pressed;
}

bool BoosterInput::isTopButtonPressed() const
{
  mutex_enter_blocking(&mutex_);
  bool pressed = buttons_[(int)ButtonType::Top].isPressed();
  mutex_exit(&mutex_);
  return pressed;
}

BoosterInput::JoystickDirection BoosterInput::getJoystickDirection() const
{
  mutex_enter_blocking(&mutex_);
  JoystickDirection dir = direction_;
  mutex_exit(&mutex_);
  return dir;
}

void BoosterInput::update()
{
  mutex_enter_blocking(&mutex_);
  
  for (int i = 0; i < BOOSTER_BUTTON_COUNT; ++i) {
    buttons_[i].update();
  }

  // Compute joystick direction based on button states
  bool up = buttons_[(int)ButtonType::Up].isPressed();
  bool down = buttons_[(int)ButtonType::Down].isPressed();
  bool left = buttons_[(int)ButtonType::Left].isPressed();
  bool right = buttons_[(int)ButtonType::Right].isPressed();

  // SOCD Cleaning: Up + Down = Neutral, Left + Right = Neutral
  if (up && down) up = down = false;
  if (left && right) left = right = false;

  JoystickDirection newDirection = JoystickDirection::Center;

  if (up && left)
    newDirection = JoystickDirection::UpLeft;
  else if (up && right)
    newDirection = JoystickDirection::UpRight;
  else if (down && left)
    newDirection = JoystickDirection::DownLeft;
  else if (down && right)
    newDirection = JoystickDirection::DownRight;
  else if (up)
    newDirection = JoystickDirection::Up;
  else if (down)
    newDirection = JoystickDirection::Down;
  else if (left)
    newDirection = JoystickDirection::Left;
  else if (right)
    newDirection = JoystickDirection::Right;

  bool isDiagonal = (newDirection == JoystickDirection::UpLeft ||
                     newDirection == JoystickDirection::UpRight ||
                     newDirection == JoystickDirection::DownLeft ||
                     newDirection == JoystickDirection::DownRight);

  bool wasDiagonal = (direction_ == JoystickDirection::UpLeft ||
                      direction_ == JoystickDirection::UpRight ||
                      direction_ == JoystickDirection::DownLeft ||
                      direction_ == JoystickDirection::DownRight);

  bool isCardinal = (newDirection == JoystickDirection::Up ||
                     newDirection == JoystickDirection::Down ||
                     newDirection == JoystickDirection::Left ||
                     newDirection == JoystickDirection::Right);

  // Apply diagonal timing when moving from center or from a diagonal to a
  // cardinal direction. This prevents accidental cardinal inputs when trying to
  // press diagonals.
  if ((direction_ == JoystickDirection::Center || wasDiagonal) && isCardinal) {
    if (lastDiagonalTime_ == 0) {
      lastDiagonalTime_ = millis();
    } else if (millis() - lastDiagonalTime_ > DIAGONAL_WINDOW_MS) {
      direction_ = newDirection;
      lastDiagonalTime_ = 0;
    }
    // Else: waiting, keep previous direction
  } else {
    // For diagonals, center, or immediate changes, update right away
    direction_ = newDirection;
    lastDiagonalTime_ = 0;
  }
  
  mutex_exit(&mutex_);
}

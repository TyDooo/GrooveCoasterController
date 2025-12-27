#include <Arduino.h>

#include "booster.h"

#define BUTTON_DEBOUNCE_MS 10
#define DIAGONAL_WINDOW_MS 25

Booster::Booster(const int buttonPins[BOOSTER_BUTTON_COUNT])
{
    for (int i = 0; i < BOOSTER_BUTTON_COUNT; ++i)
    {
        buttons_[i] = Bounce2::Button();
        buttons_[i].attach(buttonPins[i], INPUT_PULLUP);
        buttons_[i].interval(BUTTON_DEBOUNCE_MS);
        buttons_[i].setPressedState(LOW);
    }
}

bool Booster::isButtonPressed(ButtonType button) const
{
    return buttons_[(int)button].isPressed();
}

void Booster::update()
{
    for (int i = 0; i < BOOSTER_BUTTON_COUNT; ++i)
    {
        buttons_[i].update();
    }

    // Compute joystick direction based on button states
    bool up = isButtonPressed(ButtonType::Up);
    bool down = isButtonPressed(ButtonType::Down);
    bool left = isButtonPressed(ButtonType::Left);
    bool right = isButtonPressed(ButtonType::Right);

    // SOCD Cleaning: Up + Down = Neutral, Left + Right = Neutral
    if (up && down)
        up = down = false;
    if (left && right)
        left = right = false;

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

    // Apply diagonal timing when moving from center or from a diagonal to a cardinal direction.
    // This prevents accidental cardinal inputs when trying to press diagonals.
    if ((direction_ == JoystickDirection::Center || wasDiagonal) && isCardinal)
    {
        if (lastDiagonalTime_ == 0)
        {
            lastDiagonalTime_ = millis();
        }
        else if (millis() - lastDiagonalTime_ > DIAGONAL_WINDOW_MS)
        {
            direction_ = newDirection;
            lastDiagonalTime_ = 0;
        }
        // Else: waiting, keep previous direction
    }
    else
    {
        // For diagonals, center, or immediate changes, update right away
        direction_ = newDirection;
        lastDiagonalTime_ = 0;
    }
}

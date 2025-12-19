#pragma once

#include <Bounce2.h>

#define BOOSTER_BUTTON_COUNT 5

class Booster
{
public:
    enum class ButtonType
    {
        Up,
        Down,
        Left,
        Right,
        Top
    };

    Booster(const int buttonPins[BOOSTER_BUTTON_COUNT]);

    bool isButtonPressed(ButtonType button) const;
    void update();

private:
    Bounce2::Button buttons_[BOOSTER_BUTTON_COUNT];
};

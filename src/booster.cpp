#include <Arduino.h>

#include "booster.h"

#define BUTTON_DEBOUNCE_MS 10

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
}

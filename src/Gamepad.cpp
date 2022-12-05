#include "pico/stdlib.h"
#include "Gamepad.h"

// MUST BE DEFINED FOR MPG
uint32_t getMillis()
{
    return to_ms_since_boot(get_absolute_time());
}

void Gamepad::setup()
{
}

void Gamepad::read()
{
}
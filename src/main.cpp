#include "Gamepad.h"

#define GAMEPAD_DEBOUNCE_MILLIS 5

Gamepad gamepad(GAMEPAD_DEBOUNCE_MILLIS);

int main(void)
{
    gamepad.setup();

    for (;;)
    {
        static uint8_t *report;
        static const uint8_t reportSize = mpg.getReportSize();

        gamepad.read();
        gamepad.debounce();
        gamepad.process();
        report = gamepad.getReport();
    }

    return 0;
}
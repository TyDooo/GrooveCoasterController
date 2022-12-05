#include "pico/stdlib.h"
#include <MPG.h>

// MUST BE DEFINED FOR MPG
extern uint32_t getMillis();

class Gamepad : public MPG
{
public:
    Gamepad(int debounceMS = 5)
        : MPG(debounceMS) {}

    void setup();
    void read();
};

#include <MPG.h>

class Gamepad : public MPG
{
private:
    /* data */
public:
    Gamepad(int debounceMS = 5) : MPG(debounceMS) {}
    void setup() override;
    void read() override;
};
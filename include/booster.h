#pragma once

#include <Bounce2.h>
#include <FastLED.h>

#define BOOSTER_BUTTON_COUNT 5

#define NUM_LEDS 32
#define LEDS_PER_SEGMENT 4
#define NUM_SEGMENTS (NUM_LEDS / LEDS_PER_SEGMENT)

class Booster
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

  Booster(const int buttonPins[BOOSTER_BUTTON_COUNT], CRGB* leds = nullptr);

  bool isTopButtonPressed() const;
  JoystickDirection getJoystickDirection() const { return direction_; }
  void set_segment_color(int segment, CRGB color);
  void update();

 private:
  Bounce2::Button buttons_[BOOSTER_BUTTON_COUNT];
  JoystickDirection direction_;
  unsigned long lastDiagonalTime_ = 0;
  CRGB* leds_;

  bool isButtonPressed(ButtonType button) const;
};

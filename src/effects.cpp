#include "effects.h"

void RippleEffect::update()
{
  // TODO: Base on duration
  fadeToBlackBy(this->leds, NUM_LEDS, 30);

  uint16_t moveProgress = min(currentFrame, travelFrames);
  uint8_t pos = (moveProgress * maxDist) / travelFrames;

  for (int i = 0; i < NUM_SEGMENTS; i++) {
    int d1 = abs(i - center1);
    if (d1 > maxDist) d1 = NUM_SEGMENTS - d1;  // Wrap around
    int d2 = abs(i - center2);
    if (d2 > maxDist) d2 = NUM_SEGMENTS - d2;  // Wrap around
    uint8_t distance = min(d1, d2);

    if (distance == pos) {
      fill_solid(&this->leds[i * LEDS_PER_SEGMENT], LEDS_PER_SEGMENT,
                 this->color);
    }
  }

  currentFrame++;
  if (currentFrame > totalFrames) {
    active = false;
  }
}

void FadeInOutEffect::update()
{
  int newBrightness = this->brightness + this->direction;
  if (newBrightness < 0) newBrightness = 0;
  if (newBrightness > UINT8_MAX) newBrightness = UINT8_MAX;
  this->brightness = newBrightness;

  if (this->brightness == 0 || this->brightness == UINT8_MAX) {
    this->direction = -this->direction;
  }

  CRGB color = CRGB::White;
  color.fadeToBlackBy(255 - this->brightness);
  fill_solid(this->leds, NUM_LEDS, color);

  if (this->brightness == 0) this->active = false;
}

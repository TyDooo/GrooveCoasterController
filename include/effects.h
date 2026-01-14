#pragma once

#include <FastLED.h>

#include "booster.h"

#define MAX_EFFECTS 10

class Effect
{
 public:
  Effect()
  {
    active = false;
    memset(leds, 0, sizeof(leds));
  }

  bool active = false;
  virtual void update() = 0;
  CRGB leds[NUM_LEDS];
};

class EffectManager
{
  Effect* effects[MAX_EFFECTS];

 public:
  void startEffect(Effect* effect)
  {
    for (int i = 0; i < MAX_EFFECTS; i++) {
      if (!effects[i] || !effects[i]->active) {
        effects[i] = effect;
        break;
      }
    }
  }

  void updateEffects(CRGB* target_leds)
  {
    for (int i = 0; i < MAX_EFFECTS; i++) {
      if (effects[i] && effects[i]->active) {
        effects[i]->update();

        for (int j = 0; j < NUM_LEDS; j++) {
          target_leds[j] = blend(target_leds[j], effects[i]->leds[j], 128);
        }
      }
    }
  }
};

class RippleEffect : public Effect
{
 public:
  uint16_t currentFrame;
  uint16_t totalFrames;
  uint16_t travelFrames;
  int center1, center2;
  uint8_t maxDist;
  CRGB color;

  RippleEffect(int c1, int c2, uint durationMs = 200, CRGB col = CRGB::Purple)
      : center1(c1), center2(c2), color(col)
  {
    active = true;
    currentFrame = 0;
    maxDist = NUM_SEGMENTS / 2;

    totalFrames = durationMs / LED_UPDATE_INTERVAL_MS;
    if (totalFrames == 0) totalFrames = 1;

    travelFrames = (totalFrames * 8) / 10;
    if (travelFrames == 0) travelFrames = 1;
  }

  void update() override;
};

class FadeInOutEffect : public Effect
{
  uint8_t brightness;
  int8_t direction;  // Controls speed and direction of fade

 public:
  FadeInOutEffect(uint durationMs = 500)
  {
    active = true;
    brightness = 0;

    // Calculate direction based on duration
    int totalFrames = durationMs / LED_UPDATE_INTERVAL_MS;
    direction = (UINT8_MAX * 2) / totalFrames;
  }

  void update() override;
};

class HoldSolidEffect : public Effect
{
  CRGB targetColor;
  uint8_t brightness;
  uint8_t speed;
  bool released;

 public:
  HoldSolidEffect(CRGB color = CRGB::White, uint durationMs = 200)
      : targetColor(color)
  {
    active = true;
    released = false;

    speed = UINT8_MAX / (durationMs / LED_UPDATE_INTERVAL_MS);
  }

  void update() override;

  void release() { released = true; }
};

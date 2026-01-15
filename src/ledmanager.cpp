#include "ledmanager.h"

void LEDManager::setup()
{
  FastLED.addLeds<NEOPIXEL, D15>(boosterLeds[leftBooster], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, D16>(boosterLeds[rightBooster], NUM_LEDS);
}

void updateJoystickRippleEffect(BoosterInput* booster, EffectManager& manager,
                                BoosterInput::JoystickDirection& lastDir,
                                BoosterSide side)
{
  const int NUM_DIRECTIONS = 9;
  const int NUM_CENTER_SEGMENTS = 2;

  BoosterInput::JoystickDirection currentDir = booster->getJoystickDirection();

  if (lastDir == currentDir) return;

  // clang-format off
  // [Up, Down, Left, Right, UpLeft, UpRight, DownLeft, DownRight] excluding Center
  static uint8_t rippleMap[NUM_BOOSTERS][NUM_DIRECTIONS - 1][NUM_CENTER_SEGMENTS] = {
      {{5, 6}, {1, 2}, {3, 4}, {0, 7}, {4, 5}, {6, 7}, {2, 3}, {0, 1}}, // Left booster
      {{1, 2}, {5, 6}, {0, 7}, {3, 4}, {0, 1}, {2, 3}, {6, 7}, {4, 5}}  // Right booster
    };
  // clang-format on

  if ((int)lastDir >= 1 && (int)lastDir < NUM_DIRECTIONS) {
    const uint8_t* segments = rippleMap[(int)side][(int)lastDir - 1];
    manager.startEffect(new RippleEffect{segments[0], segments[1]});
  }
  lastDir = currentDir;
}

void LEDManager::updateBoosterLEDs(BoosterSide side)
{
  CRGBArray<NUM_LEDS>& leds = boosterLeds[side];

  // Clear LEDs to a dim white background
  leds.fill_solid(CHSV(0, 0, 50));

  effectManager[side].updateEffects(leds);

  updateJoystickRippleEffect(boosters[side], effectManager[side],
                             lastBoosterDir[side], side);

  if (lastTopButton[side] &&
      !boosters[side]->isTopButtonPressed()) {  // On Release
    if (!dualPressConsumed) {
      effectManager[side].startEffect(new FadeInOutEffect(CRGB::White));
    }
  }
  lastTopButton[side] = boosters[side]->isTopButtonPressed();
}

static void startHoldEffect(EffectManager& manager, HoldSolidEffect*& effect,
                            CRGB color, uint durationMs)
{
  if (effect) return;

  effect = new HoldSolidEffect(color, durationMs);
  manager.startEffect(effect);
}

static void releaseHoldEffect(HoldSolidEffect*& effect)
{
  if (effect) {
    effect->release();
    effect = nullptr;
  }
}

void LEDManager::update()
{
  const int BOTH_PRESSED_DURATION_MS = 100;

  EVERY_N_MILLISECONDS(LED_UPDATE_INTERVAL_MS)
  {
    bool currentTopButtonLeft = boosters[leftBooster]->isTopButtonPressed();
    bool currentTopButtonRight = boosters[rightBooster]->isTopButtonPressed();

    for (int i = 0; i < NUM_BOOSTERS; i++) {
      updateBoosterLEDs((BoosterSide)i);
    }

    bool bothPressed = currentTopButtonLeft && currentTopButtonRight;

    if (bothPressed) {
      if (bothPressedTime == 0) {
        bothPressedTime = millis();
      } else if (!dualPressConsumed &&
                 (millis() - bothPressedTime) >= BOTH_PRESSED_DURATION_MS) {
        const CRGB HOLD_COLOR = CRGB::DarkOrange;
        const uint HOLD_DURATION_MS = 100;

        startHoldEffect(effectManager[leftBooster], activeHold[leftBooster],
                        HOLD_COLOR, HOLD_DURATION_MS);
        startHoldEffect(effectManager[rightBooster], activeHold[rightBooster],
                        HOLD_COLOR, HOLD_DURATION_MS);

        dualPressConsumed = true;
      }
    } else {
      if (bothPressedTime != 0) {
        if (!dualPressConsumed) {
          effectManager[leftBooster].startEffect(
              new FadeInOutEffect(CRGB::Blue));
          effectManager[rightBooster].startEffect(
              new FadeInOutEffect(CRGB::Blue));
          dualPressConsumed = true;
        }

        releaseHoldEffect(activeHold[leftBooster]);
        releaseHoldEffect(activeHold[rightBooster]);

        bothPressedTime = 0;
      }
    }

    if (!currentTopButtonLeft && !currentTopButtonRight) {
      dualPressConsumed = false;
    }

    FastLED.show();
  }
}

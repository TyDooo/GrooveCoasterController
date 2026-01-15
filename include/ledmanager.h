#pragma once

#include <FastLED.h>

#include "boosterinput.h"
#include "common.h"
#include "effects.h"

#define NUM_BOOSTERS 2

enum BoosterSide { leftBooster = 0, rightBooster = 1 };

class LEDManager
{
 public:
  LEDManager(BoosterInput* left, BoosterInput* right) : boosters{left, right} {}
  void setup();
  void update();

 private:
  CRGBArray<NUM_LEDS> boosterLeds[NUM_BOOSTERS];

  // TODO: Use CRGBSet for segments?

  EffectManager effectManager[NUM_BOOSTERS];

  BoosterInput* boosters[NUM_BOOSTERS];

  BoosterInput::JoystickDirection lastBoosterDir[NUM_BOOSTERS] = {
      BoosterInput::JoystickDirection::Center,
      BoosterInput::JoystickDirection::Center};

  bool dualPressConsumed = false;
  unsigned long bothPressedTime = 0;

  bool lastTopButton[NUM_BOOSTERS] = {false, false};
  HoldSolidEffect* activeHold[NUM_BOOSTERS] = {nullptr, nullptr};

  void updateBoosterLEDs(BoosterSide index);
};
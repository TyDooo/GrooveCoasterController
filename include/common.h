#pragma once

#define NUM_LEDS 32     // Total number of LEDs per booster
#define NUM_SEGMENTS 8  // Number of segments per booster
#define LEDS_PER_SEGMENT 4

#define TARGET_FPS 100
#define LED_UPDATE_INTERVAL_MS (1000 / TARGET_FPS)

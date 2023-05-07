#ifndef _WS2812_H_
#define _WS2812_h_

#include "pico/stdlib.h"

void ws2812_init(uint8_t pin, bool is_rgbw);
void set_pixel(uint pixel_index, uint r, uint g, uint b);
void set_all_pixels(uint r, uint g, uint b);
void show_pixels();

#endif // _WS2812_H_
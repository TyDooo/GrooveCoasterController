#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "ws2812.h"

// TODO: This should come from config
#define NUM_PIXELS 16

// A buffer that holds the RGB color for each LED
static uint8_t pixel_buffer[NUM_PIXELS][3] = {0};

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void ws2812_init(uint8_t pin, bool is_rgbw)
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, pin, 800000, is_rgbw);
}

void set_pixel(uint pixel_index, uint r, uint g, uint b)
{
    if (pixel_index > NUM_PIXELS)
        return;

    pixel_buffer[pixel_index][0] = r;
    pixel_buffer[pixel_index][1] = g;
    pixel_buffer[pixel_index][2] = b;
}

void set_all_pixels(uint r, uint g, uint b)
{
    for (size_t i = 0; i < NUM_PIXELS; i++)
        set_pixel(i, r, g, b);
}

void show_pixels()
{
    for (size_t i = 0; i < NUM_PIXELS; i++)
    {
        put_pixel(urgb_u32(pixel_buffer[i][0],
                           pixel_buffer[i][1],
                           pixel_buffer[i][2]));
    }
}
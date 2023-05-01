#ifndef _BOOSTER_H_
#define _BOOSTER_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t gpio_pin;
    uint32_t gpio_mask;
} booster_btn_conf_t;

typedef struct
{
    booster_btn_conf_t up;
    booster_btn_conf_t down;
    booster_btn_conf_t left;
    booster_btn_conf_t right;
    booster_btn_conf_t btn;
} booster_conf_t;

typedef struct
{
    uint8_t gpio_pin;
    uint32_t gpio_mask;
    uint32_t last_change; // Used for debouncing
    bool active;
} booster_btn_t;

typedef struct
{
    booster_btn_t up;
    booster_btn_t down;
    booster_btn_t left;
    booster_btn_t right;
    booster_btn_t btn;
} booster_state_t;

void init_booster(booster_state_t *state, booster_conf_t *conf);
void update_booster_state(booster_state_t *state);

#endif // _BOOSTER_H_
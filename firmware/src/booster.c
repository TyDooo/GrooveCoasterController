#include "booster.h"
#include "hardware/gpio.h"
#include "pico/time.h"

// TODO: Move to config
#define BUTTON_DEBOUNCE_MS 2

static void init_booster_btn(booster_btn_t btn)
{
    gpio_init(btn.gpio_pin);
    gpio_set_dir(btn.gpio_pin, GPIO_IN);
    gpio_pull_up(btn.gpio_pin);
}

static void set_btn_conf(booster_btn_t *btn, booster_btn_conf_t conf)
{
    btn->gpio_pin = conf.gpio_pin;
    btn->gpio_mask = conf.gpio_mask;
}

void init_booster(booster_state_t *state, booster_conf_t *conf)
{
    set_btn_conf(&state->up, conf->up);
    set_btn_conf(&state->down, conf->down);
    set_btn_conf(&state->left, conf->left);
    set_btn_conf(&state->right, conf->right);
    set_btn_conf(&state->btn, conf->btn);

    init_booster_btn(state->up);
    init_booster_btn(state->down);
    init_booster_btn(state->left);
    init_booster_btn(state->right);
    init_booster_btn(state->btn);
}

static void update_booster_btn(booster_btn_t *btn)
{
    uint32_t now = to_ms_since_boot(get_absolute_time());
    bool current_state = !gpio_get(btn->gpio_pin);

    // Only update if state has changed
    if (current_state == btn->active)
        return;

    // Debounce by BUTTON_DEBOUNCE_MS to prevent mechanical jitter
    if (btn->last_change + BUTTON_DEBOUNCE_MS > now)
        return;

    btn->active = current_state;
    btn->last_change = now;
}

void update_booster_state(booster_state_t *state)
{

    update_booster_btn(&state->up);
    update_booster_btn(&state->down);
    update_booster_btn(&state->left);
    update_booster_btn(&state->right);
    update_booster_btn(&state->btn);
}
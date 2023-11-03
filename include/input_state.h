#ifndef _INPUT_STATE_H_
#define _INPUT_STATE_H_

#include "tusb.h"
#include "booster.h"

hid_gamepad_report_t get_report(booster_state_t *booster_left, booster_state_t *booster_right);

#endif // _INPUT_STATE_H_
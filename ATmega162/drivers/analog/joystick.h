
#pragma once

#include <stdint.h>

typedef struct {
    int8_t x;
    int8_t y;
} JOY_pos_t;

typedef enum {
    NEUTRAL,
    UP,
    DOWN,
    RIGHT,
    LEFT
} JOY_dir_t;


void joystick_init(void);

void JOY_set_new_center(void);

JOY_pos_t JOY_get_position(void);

JOY_dir_t JOY_get_direction(void);

int8_t JOY_get_button(void);


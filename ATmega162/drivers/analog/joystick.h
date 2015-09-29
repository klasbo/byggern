
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

void joystick_set_new_center(void);

JOY_pos_t joystick_position(void);

JOY_dir_t joystick_direction(void);

int8_t joystick_button(void);


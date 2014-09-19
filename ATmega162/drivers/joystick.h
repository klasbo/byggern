
#pragma once

typedef struct {
    int x;
    int y;
} JOY_pos_t;

typedef enum {
    NEUTRAL,
    UP,
    DOWN,
    RIGHT,
    LEFT
} JOY_dir_t;


void JOY_set_new_center(void);

JOY_pos_t JOY_get_position(void);

JOY_dir_t JOY_get_direction(void);

// TODO: Joystick button. Make part of get_direction (PRESSED)?

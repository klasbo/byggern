
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


void JOY_setNewCenter(void);

JOY_pos_t JOY_getPosition();

JOY_dir_t JOY_getDirection();

// TODO: Joystick button. Make part of getDirection (PRESSED)?

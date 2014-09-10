/*
 * joystick.h
 *
 * Created: 10.09.2014 13:33:51
 *  Author: adelaidm
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

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


void JOY_init();

void JOY_setNewCenter(void);

int Joy_button(int buttton);

JOY_pos_t JOY_getPosition();

JOY_dir_t JOY_getDirection();


#endif /* JOYSTICK_H_ */
#pragma once

#include <stdint.h>
#include "can.h"
#include "Servo/Servo.h"

typedef struct {
	int8_t		JOY_pos_x;
	int8_t		JOY_pos_y;
	uint8_t		JOY_button;
	uint8_t		slider_r;
	uint8_t		slider_l;
	uint8_t		button_r;
	uint8_t		button_l;
}ANALOG_info;


ANALOG_info unmake_msg(can_msg_t msg);

void control_servo_slider(Servo* s, uint8_t slider_pos);
void control_servo_JOY(Servo* s, int8_t JOY_pos_x);

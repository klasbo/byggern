
#pragma once

#include "joystick.h"
#include "slider.h"
#include "../communication/can/can.h"

typedef struct {
	int8_t	    JOY_pos_x;
	int8_t		JOY_pos_y;
	uint8_t		JOY_button;
	uint8_t		slider_r;
	uint8_t		slider_l;
	uint8_t		button_r;
	uint8_t		button_l;
}ANALOG_info;

ANALOG_info analog_read(void);

can_msg_t make_msg(ANALOG_info input);
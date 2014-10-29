
#include "analog_read.h"



ANALOG_info analog_read(void){
	ANALOG_info info;
	JOY_pos_t p		= JOY_get_position();
	info.JOY_pos_x	= p.x;
	info.JOY_pos_y	= p.y;
	info.JOY_button	= JOY_get_button();
	SLI_pos_t s		= SLI_get_slider_position();
	info.slider_r	= s.R;
	info.slider_l	= s.L;
	info.button_r	= SLI_get_right_button();
	info.button_l	= SLI_get_left_button();
	return info;
}


can_msg_t make_msg(ANALOG_info input){
	can_msg_t msg;
	msg.ID		= 2;
	msg.length	= 8;
	msg.data[0]	= (uint8_t)input.JOY_pos_x;
	msg.data[1]	= (uint8_t)input.JOY_pos_y;
	msg.data[2]	= input.JOY_button;
	msg.data[3]	= input.slider_r;
	msg.data[4]	= input.slider_l;
	msg.data[5]	= input.button_r;
	msg.data[6]	= input.button_l;
	return msg;
}

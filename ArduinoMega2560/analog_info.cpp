
#include "analog_info.h"

ANALOG_info unmake_msg(can_msg_t msg){
	ANALOG_info info;
	info.JOY_pos_x	= (int8_t)msg.data[0];
	info.JOY_pos_y	= (int8_t)msg.data[1];
	info.JOY_button = msg.data[2];
	info.slider_r	= msg.data[3];
	info.slider_l	= msg.data[4];
	info.button_r	= msg.data[5];
	info.button_l	= msg.data[6];
	return info;
}

void control_servo_slider(Servo* s, uint8_t slider_pos){
	s->write(180 - ((int(slider_pos)) * 7) / 10);
}

void control_servo_JOY(Servo* s, int8_t JOY_pos_x){
	s->write(90 - (JOY_pos_x*90)/100);
}

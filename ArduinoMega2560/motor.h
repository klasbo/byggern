#pragma once
#include <stdint.h>

typedef struct{
	int16_t max_r;
	int16_t max_l;
}motor_range;


#define left 0
#define right 1

void motor_init(void);


void motor_write(uint8_t speed, uint8_t dir);

motor_range motor_calibrate(void);

uint16_t motor_read(void);


void motor_set_speed(int speed);
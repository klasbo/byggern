#pragma once
#include <stdint.h>


#define left 0
#define right 1

void motor_init(void);

void motor_write(uint8_t speed, uint8_t dir);

uint16_t motor_read(void);

void motor_set_speed(int speed);
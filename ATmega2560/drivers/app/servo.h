#pragma once

#include <stdint.h>

// Set up minimum & maximum pulse width for the servo
void servo_init(float min, float max);

// Map the full int8_t range to the min..max range set by servo_init
void servo_write(int8_t val);
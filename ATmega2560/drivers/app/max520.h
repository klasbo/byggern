
#pragma once

#include <stdint.h>

void max520_init(uint8_t max520_twi_addr);
void max520_write(uint8_t ch, uint8_t val);
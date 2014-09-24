

#pragma once

#include <stdint.h>


uint8_t reverse_bits(uint8_t x);

void draw_upper_n(uint8_t b, uint8_t where_x, uint8_t where_y, uint8_t n);

void draw_lower_n(uint8_t b, uint8_t where_x, uint8_t where_y, uint8_t n);

void draw_byte(uint8_t b, uint8_t column, uint8_t page);

void render(void);

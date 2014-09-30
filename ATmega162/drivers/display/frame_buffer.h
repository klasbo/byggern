

#pragma once

#include <stdint.h>


/** Draw the contents of the frame buffer to the OLED
*/
void frame_buffer_render(void);


uint8_t reverse_bits(uint8_t x);

/// ----- Drawing bytes / pixels ----- ///
/**
Pixel coordinates start at (0, 0) in the upper left corner
All draw functions draw either a full or masked byte, which:
    - Is oriented vertically
    - Has the least significant bit on top
*/

/** Draw a full byte to pixel coordinates (x, page*8)
*/
void draw_byte(uint8_t b, uint8_t x, uint8_t page);


/** Draw a masked byte to pixel coordinates (x, y)
    Only the top n bits (n least significant bits) are drawn,
        with the LSB starting at (x, y)
*/
void draw_upper_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n);


/** Draw a masked byte to pixel coordinates (x, y)
    Only the lower n bits (n most significant bits) are drawn,
        shifted upward such that the least significant bit *after masking*
        is drawn at (x, y)
*/
void draw_lower_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n);


/** Draws a single pixel at (x, y)
    If b is 0, the pixel is erased, otherwise it is drawn.
*/
void draw_pixel(uint8_t b, uint8_t x, uint8_t y);


/// ----- Drawing shapes / areas ----- ///

/** Draws a circle with radius r at (x, y)
    Only the outline is drawn (the circle is not filled)
*/
void draw_circle(uint8_t x, uint8_t y, uint8_t r);


/** Draws a rectangle between (x0, y0) and (x1, y1)
    Only the outline is drawn (the rectangle is not filled)
*/
void draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);


/** Clears a rectangular area from (x0, y0) to (x1, y1)
*/
void clear_area(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);


/// ----- printf / Drawing characters ----- ///

void frame_buffer_printf(char* fmt, ...);

void frame_buffer_printf_P(const char* fmt, ...);

void frame_buffer_draw_char(char c);

void frame_buffer_set_font(void* addr, uint8_t width, uint8_t height, uint8_t start_offset);

void frame_buffer_set_font_spacing(uint8_t horizontal, uint8_t vertical);
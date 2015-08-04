

#pragma once

#include <stdint.h>

#include "fonts/fontdescr.h"


/** Sets the memory location of the frame buffer
*   addr is assumed to point to 512 bytes of contiguous memory
*/
void fbuf_set_addr(volatile void* addr);

/** Draw the contents of the frame buffer to the OLED
*/
void fbuf_render(void);

/** Clears and resets the frame buffer
*/
void fbuf_clear(void);


/// ----- Drawing bytes / pixels ----- ///
/**
Pixel coordinates start at (0, 0) in the upper left corner
All draw functions draw either a full or masked byte, which:
    - Is oriented vertically
    - Has the least significant bit on top
*/

/** Draw a full byte to pixel coordinates (x, page*8)
*/
void fbuf_draw_byte(uint8_t b, uint8_t x, uint8_t page);


/** Draw a masked byte to pixel coordinates (x, y)
    Only the top n bits (n least significant bits) are drawn,
        with the LSB starting at (x, y)
*/
void fbuf_draw_upper_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n);


/** Draw a masked byte to pixel coordinates (x, y)
    Only the lower n bits (n most significant bits) are drawn,
        shifted upward such that the least significant bit *after masking*
        is drawn at (x, y)
*/
void fbuf_draw_lower_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n);


/** Draws a single pixel at (x, y)
    If b is 0, the pixel is erased, otherwise it is drawn.
*/
void fbuf_draw_pixel(uint8_t b, uint8_t x, uint8_t y);


/// ----- Drawing shapes / areas ----- ///

/** Draws a circle with radius r at (x, y)
    Only the outline is drawn (the circle is not filled)
*/
void fbuf_draw_circle(uint8_t x, uint8_t y, uint8_t r);


/** Draws a rectangle between (x0, y0) and (x1, y1)
    Only the outline is drawn (the rectangle is not filled)
*/
void fbuf_draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);


/** Clears a rectangular area from (x0, y0) to (x1, y1)
*/
void fbuf_clear_area(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);


/// ----- printf / Drawing characters ----- ///

void fbuf_printf(char* fmt, ...) __attribute__((format(printf, 1, 2)));

void fbuf_printf_P(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

void fbuf_draw_char(char c);

void fbuf_set_font(FontDescr fd);

void fbuf_set_font_spacing(int8_t horizontal, int8_t vertical);

void fbuf_set_cursor_to_pixel(uint8_t x, uint8_t y);

void fbuf_set_cursor(uint8_t col, uint8_t row);


/// ----- misc ----- ///

uint8_t reverse_bits(uint8_t x);


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fonts/font8x8.h"
#include "oled.h"
#include "frame_buffer.h"

static char* frame_buffer_location;
#define frame ( (uint8_t(*)[COLUMNS]) (frame_buffer_location) )

static int              col;
static int              line;

static int              font_width;
static int              font_height;
static char             font_start_offset;
static unsigned char*   font;

static uint8_t          font_horiz_spacing;
static uint8_t          font_vert_spacing;

void __attribute__((constructor)) frame_buffer_init(void){
    frame_buffer_location = malloc(512);
}

void frame_buffer_render(void){
    for(int i = 0; i < PAGES; i++){
        OLED_go_to_line(i);
        int j;
        for(j = 0; j < COLUMNS; j++){
            *ext_oled_data = (char)frame[i][j];
        }
    }
}

void frame_buffer_clear(void){
    memset((void*)0x1800, 0, 0x400);
    col = 0;
    line = 0;
}

uint8_t reverse_bits(uint8_t x){
    x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
    x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
    x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
    return x;
}

void frame_buffer_draw_byte(uint8_t b, uint8_t column, uint8_t page){
    frame[page][column] = b;
}    

//TODO: rename!
void frame_buffer_draw_upper_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n){
    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    uint8_t mask1 = ((uint8_t)(0xff >> (8 - offset))  |  (uint8_t)(0xff << (n + offset)));
    uint8_t val1  = (uint8_t)(b << offset);

    frame[page][x] = val1 ^ ((val1 ^ frame[page][x]) & mask1);

    if(page + 1 < PAGES  &&  n + offset > 8){
        uint8_t mask2 = (uint8_t)(0xff << (offset - (8-n)));
        uint8_t val2  = (uint8_t)(b >> (8 - offset));

        frame[page+1][x]  = val2 ^ ((val2 ^ frame[page+1][x]) & mask2);
    }
}
    
void frame_buffer_draw_lower_n(uint8_t b, uint8_t x, uint8_t y, uint8_t n){
    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    uint8_t mask1 = ((uint8_t)(0xff >> (8 - offset))  |  (uint8_t)(0xff << (n + offset)));
    uint8_t val1  = (offset > (8-n)) ?
                        (uint8_t)(b << (offset - (8-n))) :
                        (uint8_t)(b >> ((8-n) - offset)) ;

    frame[page][x] = val1 ^ ((val1 ^ frame[page][x]) & mask1);

    if(page + 1 < PAGES  &&  n + offset > 8){
        uint8_t mask2 = (uint8_t)(0xff << (offset - (8-n)));
        uint8_t val2  = (uint8_t)(b >> (n - offset));

        frame[page+1][x]  = val2 ^ ((val2 ^ frame[page+1][x]) & mask2);
    }
}


void frame_buffer_draw_pixel(uint8_t b, uint8_t x, uint8_t y){
    if(x >= COLUMNS || y >= PAGES*8){ return; }

    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    if(b){
        frame[page][x] |= (1 << offset);
    } else {
        frame[page][x] &= ~(1 << offset);
    }
}

void frame_buffer_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius){
    int8_t x = radius;
    int8_t y = 0;
    int8_t radiusError = (int8_t)(1 - (int8_t)x);

    while(x >= y){
        frame_buffer_draw_pixel(1, (uint8_t)( x + x0), (uint8_t)( y + y0));
        frame_buffer_draw_pixel(1, (uint8_t)( y + x0), (uint8_t)( x + y0));
        frame_buffer_draw_pixel(1, (uint8_t)(-x + x0), (uint8_t)( y + y0));
        frame_buffer_draw_pixel(1, (uint8_t)(-y + x0), (uint8_t)( x + y0));
        frame_buffer_draw_pixel(1, (uint8_t)(-x + x0), (uint8_t)(-y + y0));
        frame_buffer_draw_pixel(1, (uint8_t)(-y + x0), (uint8_t)(-x + y0));
        frame_buffer_draw_pixel(1, (uint8_t)( x + x0), (uint8_t)(-y + y0));
        frame_buffer_draw_pixel(1, (uint8_t)( y + x0), (uint8_t)(-x + y0));
        y++;
        if (radiusError < 0){
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void frame_buffer_draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    for(uint8_t x = x0; x <= x1; x++){
        frame_buffer_draw_pixel(1, x, y0);
        frame_buffer_draw_pixel(1, x, y1);
    }
    for(uint8_t y = y0; y <= y1; y++){
        frame_buffer_draw_pixel(1, x0, y);
        frame_buffer_draw_pixel(1, x1, y);
    }
}
    
void frame_buffer_clear_area(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    uint8_t upper_page    = y0/8;
    uint8_t lower_page    = y1/8;
        
    for(uint8_t x = x0; x <= x1; x++){
        if(y1 < y0 + 8){    // less than 8 bits height => only need a single draw_upper_n command
            frame_buffer_draw_upper_n(0, x, y0, (uint8_t)(y1-y0+1));
        } else {    // more than 8 bits => fuckit, overdraw
            frame_buffer_draw_upper_n(0, x, y0, 8);
            frame_buffer_draw_upper_n(0, x, (uint8_t)(y1 - 7), 8);
            for(uint8_t page = (uint8_t)(upper_page + 1); page < lower_page; page++){
                frame_buffer_draw_byte(0, x, page);
            }
        }
    }
}


static FILE frame_buffer_stdout = FDEV_SETUP_STREAM(frame_buffer_draw_char, NULL, _FDEV_SETUP_WRITE);

void frame_buffer_printf(char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf(&frame_buffer_stdout, fmt, v);
    va_end(v);
}

void frame_buffer_printf_P(const char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf_P(&frame_buffer_stdout, fmt, v);
    va_end(v);
}

void frame_buffer_draw_char(char c){
    if(c == '\n'  &&  '\n' < font_start_offset){    // special case for newline when it is not part of the font
        for(int i = col; i < COLUMNS; i++){
            frame_buffer_draw_upper_n(0, i, line, font_height);
        }
        line = (line + font_height + font_vert_spacing) % ROWS;
        col = 0;
    } else {
        for(int i = 0; i < font_width; i++){
            frame_buffer_draw_upper_n(
                pgm_read_byte( font + (c-font_start_offset)*font_width + i ),
                col + i,
                line,
                font_height
            );
        }
        col += font_width + font_horiz_spacing; // make sure this doesn't draw stupid places in sram (fix draw_xx_n)
    }
}

void frame_buffer_set_font(void* addr, uint8_t width, uint8_t height, uint8_t start_offset){
    font                = (unsigned char*)addr;
    font_width          = width;
    font_height         = height;
    font_start_offset   = start_offset;
    frame_buffer_set_font_spacing(0, 0);
}

void frame_buffer_set_font_spacing(uint8_t horizontal, uint8_t vertical){
    font_horiz_spacing = horizontal;
    font_vert_spacing  = vertical;
}

void frame_buffer_set_cursor(uint8_t x, uint8_t y){
    col = x;
    line = y;
}
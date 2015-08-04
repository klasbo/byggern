

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "oled.h"
#include "frame_buffer.h"
#include "../memory_layout.h"

static volatile void*   fb_addr;

#define frame ( (uint8_t(*)[DISP_WIDTH]) (fb_addr) )

static uint8_t          cursor_x;
static uint8_t          cursor_y;

static uint8_t          font_width;
static uint8_t          font_height;
static char             font_start_offset;
static unsigned char*   font_addr;

static int8_t           font_horiz_spacing;
static int8_t           font_vert_spacing;

void __attribute__((constructor)) fbuf_init(void){
    fbuf_set_addr(ext_ram);
}

void fbuf_set_addr(volatile void* addr){
    fb_addr = addr;
}

void fbuf_render(void){
    for(int i = 0; i < DISP_PAGES; i++){
        oled_go_to_line(i);
        for(int j = 0; j < DISP_WIDTH; j++){
            oled_write_data((char)frame[i][j]);
        }
    }
}

void fbuf_clear(void){
    memset((void*)fb_addr, 0, 0x400);
    cursor_x = 0;
    cursor_y = 0;
}

uint8_t reverse_bits(uint8_t x){
    x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
    x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
    x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
    return x;
}

void fbuf_draw_byte(uint8_t b, uint8_t column, uint8_t page){
    frame[page][column] = b;
}


void fbuf_draw_high_bits(uint8_t b, uint8_t x, uint8_t y, uint8_t n){
    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    uint8_t mask1 = ((uint8_t)(0xff >> (8 - offset))  |  (uint8_t)(0xff << (n + offset)));
    uint8_t val1  = (uint8_t)(b << offset);

    frame[page][x] = val1 ^ ((val1 ^ frame[page][x]) & mask1);

    if(page + 1 < DISP_PAGES  &&  n + offset > 8){
        uint8_t mask2 = (uint8_t)(0xff << (offset - (8-n)));
        uint8_t val2  = (uint8_t)(b >> (8 - offset));

        frame[page+1][x]  = val2 ^ ((val2 ^ frame[page+1][x]) & mask2);
    }
}
    
void fbuf_draw_low_bits(uint8_t b, uint8_t x, uint8_t y, uint8_t n){
    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    uint8_t mask1 = ((uint8_t)(0xff >> (8 - offset))  |  (uint8_t)(0xff << (n + offset)));
    uint8_t val1  = (offset > (8-n)) ?
                        (uint8_t)(b << (offset - (8-n))) :
                        (uint8_t)(b >> ((8-n) - offset)) ;

    frame[page][x] = val1 ^ ((val1 ^ frame[page][x]) & mask1);

    if(page + 1 < DISP_PAGES  &&  n + offset > 8){
        uint8_t mask2 = (uint8_t)(0xff << (offset - (8-n)));
        uint8_t val2  = (uint8_t)(b >> (n - offset));

        frame[page+1][x]  = val2 ^ ((val2 ^ frame[page+1][x]) & mask2);
    }
}


void fbuf_draw_pixel(uint8_t b, uint8_t x, uint8_t y){
    if(x >= DISP_WIDTH || y >= DISP_HEIGHT){ return; }

    uint8_t page      = y/8;
    uint8_t offset    = y%8;

    if(b){
        frame[page][x] |= (1 << offset);
    } else {
        frame[page][x] &= ~(1 << offset);
    }
}

void fbuf_draw_circle(uint8_t x0, uint8_t y0, uint8_t radius){
    int8_t x = radius;
    int8_t y = 0;
    int8_t radiusError = (int8_t)(1 - (int8_t)x);

    while(x >= y){
        fbuf_draw_pixel(1, (uint8_t)( x + x0), (uint8_t)( y + y0));
        fbuf_draw_pixel(1, (uint8_t)( y + x0), (uint8_t)( x + y0));
        fbuf_draw_pixel(1, (uint8_t)(-x + x0), (uint8_t)( y + y0));
        fbuf_draw_pixel(1, (uint8_t)(-y + x0), (uint8_t)( x + y0));
        fbuf_draw_pixel(1, (uint8_t)(-x + x0), (uint8_t)(-y + y0));
        fbuf_draw_pixel(1, (uint8_t)(-y + x0), (uint8_t)(-x + y0));
        fbuf_draw_pixel(1, (uint8_t)( x + x0), (uint8_t)(-y + y0));
        fbuf_draw_pixel(1, (uint8_t)( y + x0), (uint8_t)(-x + y0));
        y++;
        if (radiusError < 0){
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void fbuf_draw_rectangle(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    for(uint8_t x = x0; x <= x1; x++){
        fbuf_draw_pixel(1, x, y0);
        fbuf_draw_pixel(1, x, y1);
    }
    for(uint8_t y = y0; y <= y1; y++){
        fbuf_draw_pixel(1, x0, y);
        fbuf_draw_pixel(1, x1, y);
    }
}
    
void fbuf_clear_area(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    uint8_t upper_page    = y0/8;
    uint8_t lower_page    = y1/8;
        
    for(uint8_t x = x0; x <= x1; x++){
        if(y1 < y0 + 8){    // less than 8 bits height => only need a single draw_high_bits command
            fbuf_draw_high_bits(0, x, y0, (uint8_t)(y1-y0+1));
        } else {    // more than 8 bits => fuckit, overdraw
            fbuf_draw_high_bits(0, x, y0, 8);
            fbuf_draw_high_bits(0, x, (uint8_t)(y1 - 7), 8);
            for(uint8_t page = (uint8_t)(upper_page + 1); page < lower_page; page++){
                fbuf_draw_byte(0, x, page);
            }
        }
    }
}


static FILE fbuf_stdout = FDEV_SETUP_STREAM(fbuf_draw_char, NULL, _FDEV_SETUP_WRITE);

void fbuf_printf(char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf(&fbuf_stdout, fmt, v);
    va_end(v);
}

void fbuf_printf_P(const char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf_P(&fbuf_stdout, fmt, v);
    va_end(v);
}

void fbuf_draw_char(char c){
    if(c == '\n'  &&  '\n' < font_start_offset){    // special case for newline when it is not part of the font
        for(uint8_t x = cursor_x; x < DISP_WIDTH; x++){
            fbuf_draw_high_bits(0, x, cursor_y, font_height);
        }
        cursor_y = (cursor_y + font_height + font_vert_spacing) % DISP_HEIGHT;
        cursor_x = 0;
    } else {
        for(uint8_t x = 0; x < font_width; x++){
            fbuf_draw_high_bits(
                pgm_read_byte( font_addr + (c-font_start_offset)*font_width + x ),
                cursor_x + x,
                cursor_y,
                font_height
            );
        }
        cursor_x += font_width + font_horiz_spacing; // make sure this doesn't draw stupid places in sram (fix draw_xx_n)
    }
}

void fbuf_set_font(FontDescr fd){
    font_addr           = fd.addr;
    font_width          = fd.width;
    font_height         = fd.height;
    font_start_offset   = fd.start_offset;
    fbuf_set_font_spacing(0, 0);
}

void fbuf_set_font_spacing(int8_t horizontal, int8_t vertical){
    font_horiz_spacing = horizontal;
    font_vert_spacing  = vertical;
}

void fbuf_set_cursor_to_pixel(uint8_t x, uint8_t y){
    cursor_x = x;
    cursor_y = y;
}

void fbuf_set_cursor(uint8_t col, uint8_t row){
    cursor_x = col * (font_width + font_horiz_spacing);
    cursor_y = row * (font_height + font_vert_spacing);
}

#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "oled.h"
#include "fonts/fontdescr.h"

static uint8_t page     = 0;
static uint8_t column   = 0;
static FILE oled_stdout = FDEV_SETUP_STREAM(oled_write_char, NULL, _FDEV_SETUP_WRITE);

static FontDescr font;


void oled_init(void){
    extern void memory_layout_init(void);
    memory_layout_init();

    oled_write_cmd(OLED_POWER_OFF);

    oled_write_cmd(OLED_SEGMENT_REMAP_END);

    oled_write_cmd(OLED_COM_PINS);
    oled_write_cmd(COM_PIN_ALTERNATIVE);

    oled_write_cmd(OLED_COM_SCAN_DIR_REMAPPED);

    oled_write_cmd(OLED_MUX_RATIO);
    oled_write_cmd(0x3f);                           // reset

    oled_write_cmd(OLED_DISPLAY_CLOCK);
    oled_write_cmd(0x80);                           // reset

    oled_write_cmd(OLED_CONTRAST);
    oled_write_cmd(0xf0);                           // configurable

    oled_write_cmd(OLED_PRE_CHARGE_PERIOD);
    oled_write_cmd(0x21);

    oled_write_cmd(OLED_MEMORY_ADDRESSING_MODE);
    oled_write_cmd(ADDRESSING_MODE_PAGE);

    oled_write_cmd(OLED_VCOMH_DESELECT_LEVEL);
    oled_write_cmd(VCOMH_DESELECT_0_83);

    oled_write_cmd(OLED_SELECT_IREF);
    oled_write_cmd(IREF_EXTERNAL);

    oled_write_cmd(OLED_DISPLAY_ON__FOLLOW_RAM);

    oled_write_cmd(OLED_DISPLAY_NORMAL);

    oled_write_cmd(OLED_POWER_ON);
    

    oled_reset();
}



void oled_reset(void){
    for(uint8_t i = 0; i < DISP_PAGES; i++){
        oled_clear_page(i);
    }
    oled_go_to_page(0);
    oled_go_to_column(0);
}

void oled_clear_page(uint8_t page){
    oled_go_to_page(page);
    oled_go_to_column(0);
    for(int i = 0; i < DISP_WIDTH; i++){
        oled_write_data(0);
    }
}



void oled_set_font(FontDescr fd){
    font = fd;
}



void oled_go_to_page(uint8_t p){
    page = p;
    oled_write_cmd(OLED_PMODE_PAGE_ADDRESS + page);
}

void oled_go_to_column(uint8_t c){
    column = c;
    uint8_t x = column * font.width;
    oled_write_cmd(OLED_PMODE_COLUMN_ADDRESS_LOWER + (x & 0x0f));
    oled_write_cmd(OLED_PMODE_COLUMN_ADDRESS_UPPER + ((x & 0xf0) >> 4));
}



void oled_write_char(char c){
    if(c == '\n'){
        for(uint8_t i = column*font.width; i < DISP_WIDTH; i++){
            oled_write_data(0);
        }
        page = (page + 1) % DISP_PAGES;
        oled_go_to_page(page);
        oled_go_to_column(0);
    } else {
        for (uint8_t i = 0; i < font.width; i++){
            oled_write_data(pgm_read_byte( font.addr + (c-font.start_offset)*font.width + i ));
        }
        column++;
    }
}

void oled_printf(const char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf(&oled_stdout, fmt, v);
    va_end(v);
}

void oled_printf_P(const char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vfprintf_P(&oled_stdout, fmt, v);
    va_end(v);
}



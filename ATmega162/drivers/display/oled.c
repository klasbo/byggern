
#include "../../config.h"

#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "oled.h"
#include "fonts/fontdescr.h"

static int line_number      = 0;
static int column_number    = 0;
static FILE oled_stdout = FDEV_SETUP_STREAM(oled_write_char, NULL, _FDEV_SETUP_WRITE);

static int              font_width;
static char             font_start_offset;
static unsigned char*   font_addr;


void __attribute__ ((constructor)) oled_init(void){
    extern void memory_layout_init(void);
    memory_layout_init();

    oled_write_cmd(OLED_POWER_OFF);

    oled_write_cmd(OLED_SEGMENT_REMAP_END);

    oled_write_cmd(OLED_COM_PINS);
    oled_write_cmd(OLED_COM_PIN_ALTERNATIVE);

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
    oled_go_to_column(0);
    for(int i = 0; i < DISP_PAGES; i++){
        oled_clear_line(i);
    }
    oled_go_to_line(0);
}

void oled_clear_line(int line){
    oled_go_to_line(line);
    for(int i = 0; i < DISP_WIDTH; i++){
        oled_write_data(0x74);
    }
}



void oled_set_font(FontDescr fd){
    font_addr           = fd.addr;
    font_width          = fd.width;
    font_start_offset   = fd.start_offset;
}



void oled_go_to_line(int line){
    line_number = line;
    oled_write_cmd(OLED_PAGE_START_ADDRESS + line);
}

void oled_go_to_column(int column){
    if(column < (DISP_WIDTH/font_width)){
        column_number = column;
        oled_write_cmd(OLED_COLUMN_ADDRESS);
        oled_write_cmd(column*font_width);
        oled_write_cmd(DISP_WIDTH-1);
    } else {
        // This is stupid. Asking to write outside the screen should trigger... something.
        oled_write_cmd(OLED_COLUMN_ADDRESS);
        oled_write_cmd(DISP_WIDTH-1);
        oled_write_cmd(DISP_WIDTH-1);
    }
}



void oled_write_char(char c){
    if(c == '\n'){
        for(int i = column_number*font_width; i < DISP_WIDTH; i++){
            oled_write_data(0);
        }
        line_number = (line_number + 1) % DISP_PAGES;
        oled_go_to_line(line_number);
        oled_go_to_column(0);
    } else {
        for (int i = 0; i < font_width; i++){
            oled_write_data(pgm_read_byte( font_addr + (c-font_start_offset)*font_width + i ));
        }
        column_number++;
    }
}

void oled_printf(char* fmt, ...){
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




#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "oled.h"
#include "font.h"
#include "../memory_layout.h"

#define NUM_CHARS_PER_LINE      (COLUMNS / FONT_WIDTH)

static int line_number;
static int column_number;


static char buf[NUM_CHARS_PER_LINE * PAGES];    // used as internal buffer for OLED_printf

void __attribute__ ((constructor)) OLED_init(void){
    // constructor with dependency must be triggered manually!
    extern void memory_layout_init(void);
    memory_layout_init();
    
    OLED_write_cmd(OLED_POWER_OFF);

    OLED_write_cmd(OLED_SEGMENT_REMAP_END);

    OLED_write_cmd(OLED_COM_PINS);
    OLED_write_cmd(OLED_COM_PIN_ALTERNATIVE);

    OLED_write_cmd(OLED_COM_SCAN_DIR_REMAPPED);

    OLED_write_cmd(OLED_MUX_RATIO);
    OLED_write_cmd(0x3f);                           // reset

    OLED_write_cmd(OLED_DISPLAY_CLOCK);
    OLED_write_cmd(0x80);                           // reset

    OLED_write_cmd(OLED_CONTRAST);
    OLED_write_cmd(0x50);                           // configurable

    OLED_write_cmd(OLED_PRE_CHARGE_PERIOD);
    OLED_write_cmd(0x21);

    OLED_write_cmd(OLED_MEMORY_ADDRESSING_MODE);
    OLED_write_cmd(ADDRESSING_MODE_PAGE);

    OLED_write_cmd(OLED_VCOMH_DESELECT_LEVEL);
    OLED_write_cmd(VCOMH_DESELECT_0_83);

    OLED_write_cmd(OLED_SELECT_IREF);
    OLED_write_cmd(IREF_EXTERNAL);

    OLED_write_cmd(OLED_DISPLAY_ON__FOLLOW_RAM);

    OLED_write_cmd(OLED_DISPLAY_NORMAL);

    OLED_write_cmd(OLED_POWER_ON);


    OLED_reset();
}

void OLED_reset(void){
    OLED_go_to_column(0);
    for(int i = 0; i < PAGES; i++){
        OLED_clear_line(i);
    }
    OLED_go_to_line(0);
}



void OLED_go_to_line(int line){
    line_number = line;
    OLED_write_cmd(OLED_PAGE_START_ADDRESS + line);

}

void OLED_go_to_column(int column){
    if(column < NUM_CHARS_PER_LINE){
        column_number = column;
        OLED_write_cmd(OLED_COLUMN_ADDRESS);
        OLED_write_cmd(column*FONT_WIDTH);
        OLED_write_cmd(COLUMNS-1);
    } else {
        // This is stupid. Asking to write outside the screen should trigger... something.
        OLED_write_cmd(OLED_COLUMN_ADDRESS);
        OLED_write_cmd(COLUMNS-1);
        OLED_write_cmd(COLUMNS-1);
    }
}



void OLED_print(char* s){
    for(; *s!= '\0'; s++){
        if(*s == '\n'){
            for(int i = column_number*FONT_WIDTH; i < COLUMNS; i++){
                *ext_oled_data = 0;
            }
            line_number = (line_number + 1) % FONT_HEIGHT;
            OLED_go_to_line(line_number);
            OLED_go_to_column(0);
        } else {
            OLED_write_char(*s);
        }
    }
}

void OLED_printf(char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vsprintf(buf, fmt, v);
    va_end(v);
    OLED_print(buf);
}


void OLED_clear_line(int line){
    OLED_go_to_line(line);
    for(int i = 0; i < COLUMNS; i++){
        *ext_oled_data = 0;
    }
}


void OLED_write_cmd(char c){
    *ext_oled_cmd = c;
}


void OLED_write_char(char c){
    for (int i = 0; i < FONT_WIDTH; i++){
        *ext_oled_data = pgm_read_byte(&font[c-' '][i]);
    }
    column_number++;
}





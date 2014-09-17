/*
 * oled.c
 *
 * Created: 17.09.2014 10:24:56
 *  Author: adelaidm
 */ 

#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "oled.h"
#include "font.h"

static int line_number = 0;
static int colum_number = 0;

static volatile char* oled_c = (char*)0x1000;
static volatile char* oled_d = (char*)0x1200;

void OLED_init(){
    write_c(OLED_POWER_OFF);
    write_c(OLED_SEGMENT_REMAP_END);
    write_c(OLED_COM_PINS);                 // common pads hardware: alternative
    write_c(0b00010010);                    // p29: A[4]=1, A[5]=0
    write_c(OLED_COM_SCAN_DIR_REMAPPED);
    write_c(OLED_MUX_RATIO);
    write_c(0b00111111);                    // reset
    write_c(OLED_DISPLAY_CLOCK);            // display divide ratio/osc. freq. mode
    write_c(0b10000000);                    // reset
    write_c(OLED_CONTRAST);                 // contrast control
    write_c(0x50);                          // configurable
    write_c(OLED_PRE_CHARGE_PERIOD);
    write_c(0x21);                          // ??
    write_c(OLED_MEMORY_ADDRESSING_MODE);
    write_c(0b00000010);                    // Page addressing mode / reset
    write_c(OLED_VCOMH_DESELECT_LEVEL);
    write_c(0b00110000);                    // 0.83 * Vcc
    write_c(OLED_SELECT_IREF);
    write_c(0x00);                          // External Iref
    write_c(OLED_DISPLAY_RAM);              // out follows RAM content
    write_c(OLED_DISPLAY_NORMAL);           // set normal display
    write_c(OLED_POWER_ON);                 // display on
    
    
    OLED_reset();
}


void write_c(char c){    
    *oled_c = c;
}

void write_d(char d){
    for (int i=0;i<FONT_WIDTH;i++){
        *oled_d = pgm_read_byte(&font[d-' '][i]);
    }
}

void OLED_print(char* s){
    for(; *s!= '\0'; s++){
        if(*s == '\n'){
            line_number = (line_number + 1) % FONT_HEIGHT;
            OLED_go_to_line(line_number);
            OLED_go_to_column(0);
        } else {
            write_d(*s);
        }
    }
}

static char buf[NUM_CHARS_PER_LINE * PAGES];

void OLED_printf(char* fmt, ...){
    va_list v;
    va_start(v, fmt);
    vsprintf(buf, fmt, v);
    va_end(v);
    OLED_print(buf);
}


void OLED_go_to_line(int line){
    line_number = line;
    write_c(OLED_PAGE_START_ADDRESS + line);
    
}

void OLED_go_to_column(int column){
    // TODO: if column valid with this font
    if(column < NUM_CHARS_PER_LINE){
        colum_number = column;
        write_c(OLED_COLUMN_ADDRESS);
        write_c(column*FONT_WIDTH);
        write_c(COLUMNS-1);
    } else {
        write_c(OLED_COLUMN_ADDRESS);
        write_c(COLUMNS-1);
        write_c(COLUMNS-1);
    }        
}

void OLED_clear_line(int line){    
    OLED_go_to_line(line);
    for(int i = 0; i < COLUMNS; i++){
        *oled_d = 0;
    }
}

void OLED_reset(void){
    OLED_go_to_column(0);
    for(int i = 0; i < PAGES; i++){
        OLED_clear_line(i);
    }
    OLED_go_to_line(0);
}

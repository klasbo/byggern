/*
 * oled.h
 *
 * Created: 17.09.2014 10:24:43
 *  Author: adelaidm
 */ 


#ifndef OLED_H_
#define OLED_H_


#define ROWS                        64
#define COLUMNS                     128
#define PAGES                       8
#define PAGE_WIDTH                  (ROWS/8)
#define FONT_WIDTH                  8
#define NUM_CHARS_PER_LINE          (COLUMNS / FONT_WIDTH)
#define MAX_PAGE                    (PAGES - 1)
#define MAX_COL                     (COLUMNS - 1)

#define OLED_RESET                  0x08

#define HORIZONTAL_ADDRESSING_MODE  0x00
#define VERTICAL_ADDRESSING_MODE    0x01
#define PAGE_ADDRESSING_MODE        0x02
#define OLED_MEMORY_ADDRESSING_MODE 0x20

#define OLED_COLUMN_ADDRESS         0x21
#define OLED_PAGE_ADDRESS           0x22
#define OLED_CONTRAST               0x81
#define OLED_SEGMENT_REMAP_START    0xA0
#define OLED_SEGMENT_REMAP_END      0xA1
#define OLED_DISPLAY_RAM            0xA4
#define OLED_DISPLAY_ON             0xA5

#define OLED_DISPLAY_NORMAL         0xA6
#define OLED_DISPLAY_INVERSE        0xA7

#define OLED_MUX_RATIO              0xA8

#define IREF_INTERNAL               0x00
#define IREF_EXTERNAL               0x10
#define OLED_SELECT_IREF            0xAD

#define OLED_POWER_OFF              0xAE
#define OLED_POWER_ON               0xAF
#define OLED_CMD_MODE               0x80
#define OLED_DATA_MODE              0x40

#define VCOMH_DESELECT_0_65         0x00
#define VCOMH_DESELECT_0_77         0x20
#define VCOMH_DESELECT_0_83         0x30
#define OLED_VCOMH_DESELECT_LEVEL   0xDB

#define PAGE0                       0x00
#define PAGE1                       0x01
#define PAGE2                       0x02
#define PAGEE                       0x03
#define PAGE4                       0x04
#define PAGE5                       0x05
#define PAGE6                       0x06
#define PAGE7                       0x07
#define OLED_PAGE_START_ADDRESS     0xB0

#define OLED_DISPLAY_OFFSET         0xD3
#define OLED_DISPLAY_CLOCK          0xD5
#define OLED_PRE_CHARGE_PERIOD      0xD9
#define OLED_COM_PINS               0xDA
#define OLED_COM_SCAN_DIR_NORMAL    0xC0
#define OLED_COM_SCAN_DIR_REMAPPED  0xC8




void OLED_init(void);

void write_c(char c);

void write_d(char d);

void OLED_print(char* s);

void OLED_printf(char* fmt, ...);


void OLED_clear_line(int line);

void OLED_reset(void);


void OLED_go_to_line(int line);

void OLED_go_to_column(int column);




#endif /* OLED_H_ */
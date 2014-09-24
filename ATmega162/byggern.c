
#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "drivers/uart.h"
#include "drivers/adc.h"
#include "drivers/joystick.h"
#include "drivers/slider.h"
#include "drivers/display/oled.h"
#include "drivers/memory_layout.h"
#include "drivers/display/font.h"
#include "drivers/display/frame_buffer.h"
#include "menu/menu.h"
#include "drivers/spi/spi.h"
#include "drivers/mcp2515/mcp2515.h"
#include "drivers/can/can.h"




void SRAM_test(void);

// TODO: sram.c with SRAM_test, SRAM_clear
//   region allocator? Prevent ovverwriting the frame buffer

int main(void){
    TCCR3B |= 1<<(CS30);
    
    can_msg_t msg1;
    can_msg_t msg2;
    msg1.ID = 9;
    msg1.length = 8;
    for(int i = 0; i < 8; i++){
        msg1.data[i] = i*i;
    }

    while(1){
        CAN_send(msg1);
        msg2 = CAN_recv();
        printf("msg1: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n",
            msg1.ID,
            msg1.length,
            msg1.data[0], msg1.data[1], msg1.data[2], msg1.data[3],
            msg1.data[4], msg1.data[5], msg1.data[6], msg1.data[7]
        );
        printf("msg2: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n",
            msg2.ID,
            msg2.length,
            msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3], 
            msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
        );
        printf("\n");
        msg1.data[0]++;
        
        _delay_ms(500);
        
    }

    /*
    JOY_set_new_center();

    menunode_t* menu = get_menu();
    menunode_t* prev_menu = menu;
    JOY_dir_t dirn = JOY_get_direction();
    JOY_dir_t prev_dirn = dirn;

    OLED_printf("Menu:\n  %s", menu->item.name);
    while(1){
        JOY_dir_t dirn = JOY_get_direction();
        if(prev_dirn != dirn){
            prev_dirn = dirn;
            switch(dirn){
                case RIGHT: menu = menu_open(menu);     break;
                case LEFT:  menu = menu_close(menu);    break;
                case DOWN:  menu = menu_next(menu);     break;
                case UP:    menu = menu_prev(menu);     break;
                default: break;
            }
        }
        if(prev_menu != menu){
            prev_menu = menu;
            OLED_reset();
            OLED_printf("Menu:\n %s", menu->item.name);
        }
        _delay_ms(20);
    }
    */


    /*
    SRAM_test();
    
    JOY_set_new_center();
    JOY_pos_t joy_pos;
    SLI_pos_t sli_pos;
    
    int c = 0;
    
    
    for (int i = 0; i < 0x800; i++){
        ext_ram[i] = 0;
    }
    
    OLED_printf("test\nmultiline\n  line3\n      right edge\n\n\n\nlower line");
    
    _delay_ms(1000);    
    
    for(int i = 0; i < FONT_WIDTH; i++){
        draw_upper_n(pgm_read_byte(&font['f'-' '][i]), i+100, 30, 5);        
        draw_lower_n(pgm_read_byte(&font['g'-' '][i]), i,     44, 4);
    }
    
    for(int i = 40; i < 80; i++){
        draw_upper_n(1, i, 20, 1);
        draw_upper_n(1, i, 40, 1);
    }
    for(int i = 20; i < 40; i++){
        draw_upper_n(1, 40, i, 1);
        draw_upper_n(1, 80, i, 1);
    }
    
    
    
    OLED_reset();
    render();
    
    _delay_ms(2000);
    
    OLED_printf("\nnew\nmultiline\ntest\nthing\n");
    printf("timer tick, 3: %d\n", (uint16_t)(TCNT3H<<8 | TCNT3L) );
    
    SRAM_test();
    */


        
    /*
    while(1){
        
        sli_pos = SLI_get_slider_position();
        printf("Slider position: (Left:%d, Right:%d)\n", sli_pos.L, sli_pos.R);
        printf("Slider buttons:  (%s, %s)\n", (SLI_get_left_button() ? "LEFT" : "Left"), (SLI_get_right_button() ? "RIGHT" : "Right") );
        joy_pos = JOY_get_position();
        printf("pos_x: %4d  |  pos_y: %4d\n", joy_pos.x, joy_pos.y);
        printf("Joy dirn: %d\n", JOY_get_direction());
        
                
        printf("\n\n");
        
        OLED_printf(
            "hello world\n"
            " iteration %d\n\n", c++
        );
        
                
        _delay_ms(1000);
    }
    */
}


void SRAM_test(void){
    unsigned int i, werrors, rerrors;
    werrors = 0;
    rerrors = 0;
    unsigned char testvalue;
    printf("Starting SRAM test...\r\n");
    
    uint16_t seed = (TCNT3H<<8 | TCNT3L);
    srandom(seed);
    
    for (i = 0; i < 0x800; i++) {
        testvalue = random();
        ext_ram[i] = testvalue;
        if (ext_ram[i] != testvalue) {
            printf("SRAM error (write phase): ext_ram[%4d] = %02X (should be %02X)\n", i, ext_ram[i], testvalue);
            werrors++;
        }
    }
    srandom(seed);
    for (i = 0; i < 0x800; i++) {
        testvalue = random();
        if (ext_ram[i] != testvalue) {
            printf("SRAM error (read phase): ext_ram[%4d] = %02X (should be %02X)\n", i, ext_ram[i], testvalue);
            rerrors++;
        }
    }
    printf("SRAM test completed with:\n  Read phase errors:  %4d\n  Write phase errors: %4d\n", werrors, rerrors);
}


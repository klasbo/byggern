
#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "drivers/analog/joystick.h"
#include "drivers/analog/slider.h"
#include "drivers/display/oled.h"
#include "drivers/display/frame_buffer.h"
#include "drivers/display/fonts/font8x8.h"
#include "drivers/memory_layout.h"
#include "drivers/communication/can/can.h"
#include "menu/menu.h"
#include "userprofile/userprofile.h"
#include "macros.h"



extern void createDefaultProfile(void);

int main(void){
    //createDefaultProfile();
    // TODO: move this somewhere...
    TCCR3B |= 1<<(CS30);
    
    printf_P(PSTR("\nstarted!\n"));
    
    //frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
    //frame_buffer_set_font_spacing(-2, 4);
    //frame_buffer_printf_P(PSTR("frame buffer\nmultiline\ntest\n  yay?"));
    //frame_buffer_render();
    
    //_delay_ms(5000);


    menunode_t* menu        = menu_open(get_menu());
    menunode_t* prev_menu   = 0;
    
    JOY_dir_t   dirn        = JOY_get_direction();

    
    while(1){
        if_assignment_modifies(dirn, JOY_get_direction()){
            switch(dirn){
                case RIGHT: menu = menu_open(menu);     break;
                case LEFT:  menu = menu_close(menu) == get_menu() ? 
                                    menu :
                                    menu_close(menu);
                            break;
                case DOWN:  menu = menu_next(menu);     break;
                case UP:    menu = menu_prev(menu);     break;
                default: break;
            }
        }

        if_assignment_modifies(prev_menu, menu){            
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_clear();
            frame_buffer_printf("%s\n", menu_close(menu)->item.name);
            for(int idx = 0; idx < menu_close(menu)->num_submenus; idx++){
                if(menu_open_submenu(menu_close(menu), idx) == menu){
                    frame_buffer_printf("> ");
                } else {
                    frame_buffer_printf("  ");
                }
                frame_buffer_printf("%s\n", menu_open_submenu(menu_close(menu), idx)->item.name);
            }
            if(menu->item.fun){
                frame_buffer_set_cursor(10*FONT8x8_WIDTH, 7*FONT8x8_HEIGHT);
                frame_buffer_printf("[Open]");
            }
            frame_buffer_render();
        }
		
		
        if(menu->item.fun  &&  SLI_get_right_button()){
            menu->item.fun();
            prev_menu = NULL;   // hack to make the menu draw again
        }
    }
    
}





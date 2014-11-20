
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
#include "menu/menu.h"
#include "drivers/communication/can/can.h"
#include "drivers/communication/uart.h"

#include "userprofile/userprofile.h"
#include "macros.h"



extern void createDefaultProfile(void);

int main(void){
    // Hack: The option to "Preserve EEPROM" doesn't actually preserve EEPROM
    createDefaultProfile();
    
    menunode_t* menu        = menu_open(get_menu());
    menunode_t* prev_menu   = 0;
    
    JOY_dir_t   dirn        = JOY_get_direction();
    
    
    while(1){
        if_assignment_modifies(dirn, JOY_get_direction()){
            switch(dirn){
                case LEFT:
                    if(menu_close(menu) != get_menu()){
                        menu = menu_close(menu);
                    }
                    break;
                case RIGHT: menu = menu_open(menu);     break;
                case DOWN:  menu = menu_next(menu);     break;
                case UP:    menu = menu_prev(menu);     break;
                default: break;
            }
        }
        
        if_assignment_modifies(prev_menu, menu){
            // Set the font each time, because a program may have modified it.
            frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
            frame_buffer_set_font_spacing(-1, 0);
            frame_buffer_clear();

            // Parent name
            frame_buffer_printf("%s\n", menu_close(menu)->item.name);

            // Submenu names
            foreach_submenu(menu_close(menu), lambda(void, (menunode_t* m, __attribute__((unused)) int8_t idx){
                frame_buffer_printf("  %s\n", m->item.name);
            }));
            
            // Indicate which item is selected
            frame_buffer_set_cursor(0, (menu_index(menu)+1)*FONT8x8_HEIGHT);
            frame_buffer_printf(">");
            
            // Indicate runnable program
            if(menu->item.fun){
                frame_buffer_set_cursor(10*FONT8x8_WIDTH, 7*FONT8x8_HEIGHT-1);
                frame_buffer_printf("[Open]");
            }

            frame_buffer_render();
        }

        if(SLI_get_right_button() && menu->item.fun){
            menu->item.fun();
            prev_menu = NULL;
        }
    }

    return 0;
    
}





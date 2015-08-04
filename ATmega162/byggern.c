
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
            fbuf_set_font(font8x8());
            fbuf_set_font_spacing(-1, 0);
            fbuf_clear();

            // Parent name
            fbuf_printf("%s\n", menu_close(menu)->item.name);

            // Submenu names
            foreach_submenu(menu_close(menu), lambda(void, (menunode_t* m, __attribute__((unused)) int8_t idx){
                fbuf_printf("  %s\n", m->item.name);
            }));
            
            // Indicate which item is selected
            fbuf_set_cursor(0, menu_index(menu)+1);
            fbuf_printf(">");
            
            // Indicate runnable program
            if(menu->item.fun){
                fbuf_set_cursor(10, 7);
                fbuf_printf("[Open]");
            }

            fbuf_render();
        }

        if(SLI_get_right_button() && menu->item.fun){
            menu->item.fun();
            prev_menu = NULL;
        }
    }

    return 0;
    
}





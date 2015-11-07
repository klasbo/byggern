
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
#include "drivers/display/fonts/font5x7w.h"
#include "drivers/memory_layout.h"
#include "menu/menu.h"
#include "../lib/can/can.h"
#include "../lib/can/can_test.h"
#include "../lib/uart/uart.h"

#include "userprofile/userprofile.h"
#include "../lib/macros.h"



int main(void){    
    uart_init();
    joystick_init();
    slider_init();
    oled_init();
    fbuf_set_addr(malloc(1024));
    can_init();

    printf("started\n");
    
    menunode_t* menu        = menu_open(get_menu());
    menunode_t* prev_menu   = 0;
    
    JOY_dir_t   dirn        = joystick_direction();
    
    
    while(1){
        if_assignment_modifies(dirn, joystick_direction()){
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
            fbuf_set_font(font5x7w());
            fbuf_set_font_spacing(1, 1);
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
                fbuf_set_cursor_to_pixel(
                    DISP_WIDTH - fbuf_get_current_font_config().width*6, 
                    DISP_HEIGHT-8
                );
                fbuf_printf("[Open]");
            }

            fbuf_render();
        }

        if(slider_right_button() && menu->item.fun){
            menu->item.fun();
            prev_menu = NULL;
        }
    }

    return 0;
    
}





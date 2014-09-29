
#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "drivers/analog/joystick.h"
#include "drivers/analog/slider.h"
#include "drivers/display/oled.h"
#include "drivers/display/frame_buffer.h"
#include "drivers/memory_layout.h"
#include "menu/menu.h"

#include "drivers/communication/can/can.h"


#define if_assignment_modifies(lval, rval) \
    typeof(lval) _##lval = lval; \
    if( (_##lval = lval), ((lval = rval) != _##lval) )


// TODO: SRAM region allocator? Prevent overwriting the frame buffer



int main(void){
    
    // TODO: move this somewhere...
    TCCR3B |= 1<<(CS30);
    
    printf("\nstarted!\n");

    

    menunode_t* menu        = get_menu();
    menunode_t* prev_menu   = 0;
    
    JOY_dir_t   dirn        = JOY_get_direction();
    
    while(1){
        if_assignment_modifies(dirn, JOY_get_direction()){
            switch(dirn){
                case RIGHT: menu = menu_open(menu);     break;
                case LEFT:  menu = menu_close(menu);    break;
                case DOWN:  menu = menu_next(menu);     break;
                case UP:    menu = menu_prev(menu);     break;
                default: break;
            }
        }

        if_assignment_modifies(prev_menu, menu){
            OLED_reset();
            foreach_parent_reverse(menu, ({
                void foo(menunode_t* m, int lvl){
                    for(int i = 0; i < lvl*2; i++){
                        OLED_printf_P(PSTR(" "));
                    }
                    OLED_printf_P(PSTR("%s\n"), m->item.name);
                } &foo; })
            );
        }
        
        //TODO: use joystick button
        if(SLI_get_right_button() && menu->item.fun){
            OLED_printf("\nopening..\n");
            menu->item.fun();
            OLED_printf("\ndone.\n");
        }

        //TODO: delay until / periodic
        _delay_ms(20);
    }
}






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
#include "drivers/display/font8x8.h"
#include "drivers/memory_layout.h"
#include "menu/menu.h"
#include "drivers/communication/can/can.h"
#include "drivers/communication/uart.h"
#include "drivers/analog/analog_read.h"


#define if_assignment_modifies(lval, rval) \
    typeof(lval) _##lval = lval; \
    if( (_##lval = lval), ((lval = rval) != _##lval) )

#define lambda(returnType, body) \
({ \
    returnType __fn body \
    &__fn; \
})

// TODO: SRAM region allocator? Prevent overwriting the frame buffer


extern void can_test(void);

#define CAN_JOYSTICK_ID 3
#define CAN_JOYSTICK_LENGTH 2

can_msg_t JOY_CAN_msg(void){
    can_msg_t msg;
    msg.ID      = CAN_JOYSTICK_ID;
    msg.length  = CAN_JOYSTICK_LENGTH;
    JOY_pos_t p = JOY_get_position();
    msg.data[0] = p.x;
    msg.data[1] = p.y;
    return msg;
}


int main(void){
	
    // TODO: move this somewhere...
    TCCR3B |= 1<<(CS30);
    
    printf_P(PSTR("\nstarted!\n"));
    
	ANALOG_info info;
	can_msg_t msg;
	while(1){
		info = analog_read();
		printf("%3d\t%3d\n", info.JOY_pos_x, info.JOY_pos_y);
		msg = make_msg(info);
		CAN_send(msg);
		_delay_ms(20);
	}
	
	
	
    /*
    can_msg_t msg;
    msg.ID = 10;
    msg.length = 7;
    msg.data[0] = 'y';
    msg.data[1] = 'a';
    msg.data[2] = 'y';
    msg.data[3] = '!';
    msg.data[4] = ' ';
    msg.data[5] = 0;
    msg.data[6] = 0;
    
    int iter = 0;
    while(1){
        sprintf((char*)&msg.data[5], "%d", iter++);
        printf("sending... %s\n", msg.data);
        CAN_send(msg);
        printf("CAN send string complete\n");

        CAN_send(JOY_CAN_msg());
        printf("CAN send joystick msg complete\n");

        _delay_ms(2000);
    }
    */

    
    
    //frame_buffer_set_font(font8x8, FONT8x8_WIDTH, FONT8x8_HEIGHT, FONT8x8_START_OFFSET);
    //frame_buffer_set_font_spacing(-2, 4);
    //frame_buffer_printf_P(PSTR("frame buffer\nmultiline\ntest\n  yay?"));
    //frame_buffer_render();
    
    //_delay_ms(5000);

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
            foreach_parent_reverse(menu, lambda(void, (menunode_t* m, int lvl){
                    for(int i = 0; i < lvl; i++){
                        OLED_printf_P(PSTR("  "));
                    }
                    OLED_printf_P(PSTR("%s\n"), m->item.name);
                })
            );
        }
		
        //TODO: use joystick button
		
        if(SLI_get_right_button() && menu->item.fun){
            OLED_reset();
            OLED_printf("\nopening..\n");
            menu->item.fun();
            OLED_printf("\ndone.\n");
        }
		
		
        //TODO: delay until / periodic
        _delay_ms(20);
    }
    
}





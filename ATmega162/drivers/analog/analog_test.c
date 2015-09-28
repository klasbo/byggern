
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "joystick.h"
#include "slider.h"


void analog_test(void){
    JOY_set_new_center();
    
    JOY_pos_t joy_pos;
    SLI_pos_t sli_pos;

    while(1){
        sli_pos = SLI_get_slider_position();

        printf_P(PSTR("Slider position: (Left:%d, Right:%d)\n"),
            sli_pos.L, sli_pos.R);

        printf_P(PSTR("Slider buttons:  (%s, %s)\n"),
            (SLI_get_left_button()  ? "LEFT"  : "Left"),
            (SLI_get_right_button() ? "RIGHT" : "Right") );

        joy_pos = JOY_get_position();

        printf_P(PSTR("pos_x: %4d  |  pos_y: %4d\n"),
            joy_pos.x, joy_pos.y);

        printf_P(PSTR("Joy dirn: %d\n"),
            JOY_get_direction());

        printf_P(PSTR("Joy button: %d\n\n\n"),
            JOY_get_button());
        
        _delay_ms(1000);
    }
}

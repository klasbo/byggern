
#include "joystick.h"
#include "avr/io.h"

#define DIR_THRESHOLD 80

static uint8_t center_x;
static uint8_t center_y;

void __attribute__ ((constructor)) joystick_init(void){
}

void JOY_set_new_center(void){
}

JOY_pos_t JOY_get_position(void){

    return (JOY_pos_t){
        .x = 0,
        .y = 0
    };
}


JOY_dir_t JOY_get_direction(void){
    JOY_pos_t pos = JOY_get_position();
        
    return
    pos.x >  DIR_THRESHOLD  ? RIGHT  :
    pos.x < -DIR_THRESHOLD  ? LEFT   :
    pos.y >  DIR_THRESHOLD  ? UP     :
    pos.y < -DIR_THRESHOLD  ? DOWN   :
                              NEUTRAL;
}

int8_t JOY_get_button(void){
    return 0;
}


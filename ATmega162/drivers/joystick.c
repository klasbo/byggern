/*
 * joystick.c
 *
 * Created: 10.09.2014 13:33:35
 *  Author: adelaidm
 */ 


#include "joystick.h"
#include "adc.h"

#define DIR_THRESHOLD 20

static int center_x;
static int center_y;


void JOY_setNewCenter(void){
    center_x = ADC_read(JOY_X);
    center_y = ADC_read(JOY_Y);
    return;
    
};

JOY_pos_t JOY_getPosition(void){
    unsigned int x_pos_u = ADC_read(JOY_X);
    unsigned int y_pos_u = ADC_read(JOY_Y);
    int x_pos;
    int y_pos;
    //if the ADC reads a larger number than the range of an unsigned int
    // you subtract 127 first and then typecast to signed int
    
    if(x_pos_u>127){
        x_pos_u -=  127;
        x_pos   =   (signed int)x_pos_u;
    }
    else{
        x_pos   =   (signed int)x_pos_u;
        x_pos   -=  127;
    }
    if(y_pos_u>127){
        y_pos_u -=  127;
        y_pos   =   (signed int)y_pos_u;
    }
    else{
        y_pos   =   (signed int)y_pos_u;
        y_pos   -=  127;
    }
    
    return (JOY_pos_t){
            .x = x_pos + (127 - center_x),
            .y = y_pos + (127 - center_y)
        };
};


JOY_dir_t JOY_getDirection(void){
    JOY_pos_t pos = JOY_getPosition();
        
    return
    pos.x >  DIR_THRESHOLD  ? RIGHT  :
    pos.x < -DIR_THRESHOLD  ? LEFT   :
    pos.y >  DIR_THRESHOLD  ? UP     :
    pos.y < -DIR_THRESHOLD  ? DOWN   :
                              NEUTRAL;
}



#include "joystick.h"
#include "adc.h"

#define DIR_THRESHOLD 20

static uint8_t center_x;
static uint8_t center_y;


void JOY_setNewCenter(void){
    center_x = ADC_read(JOY_X);
    center_y = ADC_read(JOY_Y);
};

JOY_pos_t JOY_getPosition(void){

    uint8_t x_pos_u = ADC_read(JOY_X);
    uint8_t y_pos_u = ADC_read(JOY_Y);
    
    #define saturated_subtract(result, a, b) \
        int8_t result = \
            b > 128 && (uint8_t)(127 + b)  >=  a    ?   -128    : \
            b < 128 && (uint8_t)(128 + b)  <=  a    ?   127     : \
                                                        (int8_t)(a - b);
                                                            
    saturated_subtract(x_pos, x_pos_u, center_x)
    saturated_subtract(y_pos, y_pos_u, center_y)
    
    return (JOY_pos_t){
        .x = x_pos,
        .y = y_pos
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


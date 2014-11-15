#include "joystick.h"
#include "adc.h"
#include "avr/io.h"

#define DIR_THRESHOLD       80
#define NEUTRAL_THRESHOLD   40

static uint8_t center_x;
static uint8_t center_y;

void __attribute__ ((constructor)) joystick_init(void){
    extern void adc_init(void);
    adc_init();
    JOY_set_new_center();
    PORTB &= ~(1 << PINB2);
}

void JOY_set_new_center(void){
    center_x = ADC_read(JOY_X);
    center_y = ADC_read(JOY_Y);
}

JOY_pos_t JOY_get_position(void){

    uint8_t x_pos_u = ADC_read(JOY_X);
    uint8_t y_pos_u = ADC_read(JOY_Y);
    
    #define saturated_subtract(result, a, b) \
        int8_t result = \
            b > 128 && (uint8_t)(127 + b)  >=  a    ?   -128    : \
            b < 128 && (uint8_t)(128 + b)  <=  a    ?   127     : \
                                                        (int8_t)(a - b);
    
    #define saturated_subtract2(result, input,center)\
        int8_t result=\
            (input<center) ?   -((center-input)*100)/(center) :\
            ((input-center)*100)/(255-center);
                                                       
    saturated_subtract2(x_pos, x_pos_u, center_x)
    saturated_subtract2(y_pos, y_pos_u, center_y)
    
    return (JOY_pos_t){
        .x = x_pos,
        .y = y_pos
    };
}


JOY_dir_t JOY_get_direction(void){
    static JOY_dir_t previous;
    
    JOY_pos_t pos = JOY_get_position();
            
    /* To prevent "bouncing" we store the previous direction:
        If the position is in the deadzone between NEUTRAL_THRESHOLD and 
        DIR_THRESHOLD, the direction is "undecided", so we return the previous direction
    */
    JOY_dir_t d =
    pos.x >  DIR_THRESHOLD  ? RIGHT  :  // Outside the "direction" square
    pos.x < -DIR_THRESHOLD  ? LEFT   :
    pos.y >  DIR_THRESHOLD  ? UP     :
    pos.y < -DIR_THRESHOLD  ? DOWN   :
    pos.x <  NEUTRAL_THRESHOLD &&       // Within the "neutral" square
    pos.x > -NEUTRAL_THRESHOLD &&
    pos.y <  NEUTRAL_THRESHOLD &&
    pos.y > -NEUTRAL_THRESHOLD 
                            ? NEUTRAL :
                            previous;   // In the deadzone
    previous = d;
    return d;
}

int8_t JOY_get_button(void){
    return !(PINB & (1<<PINB2));
}

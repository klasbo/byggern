#include "joystick.h"
#include "adc.h"
#include "avr/io.h"

#define DIR_THRESHOLD       80
#define NEUTRAL_THRESHOLD   40

static uint8_t center_x;
static uint8_t center_y;

void joystick_init(void){
    extern void adc_init(void);
    adc_init();
    joystick_set_new_center();
    DDRB &= ~(1 << DDB2);
}

void joystick_set_new_center(void){
    center_x = adc_read(JOY_X);
    center_y = adc_read(JOY_Y);
}

JOY_pos_t joystick_position(void){
    int16_t x16 = ((int16_t)adc_read(JOY_X) - (int16_t)center_x);
    int16_t y16 = ((int16_t)adc_read(JOY_Y) - (int16_t)center_y);
    
    return (JOY_pos_t){
        .x =    (x16 > 127)     ? 127   : 
                (x16 < -128)    ? -128  : 
                                  (int8_t)x16,
        .y =    (y16 > 127)     ? 127   : 
                (y16 < -128)    ? -128  : 
                                  (int8_t)y16,
    };
}


JOY_dir_t joystick_direction(void){
    static JOY_dir_t previous;
    
    JOY_pos_t pos = joystick_position();
            
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

int8_t joystick_button(void){
    return !(PINB & (1<<PINB2));
}

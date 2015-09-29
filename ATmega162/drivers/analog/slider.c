
#include <avr/io.h>

#include "slider.h"
#include "adc.h"

void slider_init(void){
    extern void adc_init(void);
    adc_init();
    DDRB &= ~ (1<<DDB1);
    DDRB &= ~ (1<<DDB0);
}    

SLI_pos_t slider_position(void){
    return (SLI_pos_t){
        .L = adc_read(SLIDER_L),
        .R = adc_read(SLIDER_R)
    };
}

int slider_right_button(void){
    return !!(PINB & (1<<PINB1));
}    

int slider_left_button(void){    
    return !!(PINB & (1<<PINB0));
}    

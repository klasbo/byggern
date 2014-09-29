
#include <avr/io.h>

#include "slider.h"
#include "adc.h"

void __attribute__ ((constructor)) slider_init(void){
    DDRB &= ~ (1<<DDB1);
    DDRB &= ~ (1<<DDB0);
}    

SLI_pos_t SLI_get_slider_position(void){
    return (SLI_pos_t){
        .L = ADC_read(SLIDER_L),
        .R = ADC_read(SLIDER_R)
    };
}

int SLI_get_right_button(void){
    return !!(PINB & (1<<PINB1));
}    

int SLI_get_left_button(void){    
    return !!(PINB & (1<<PINB0));
}    

/*
 * slider.c
 *
 * Created: 10.09.2014 17:08:23
 *  Author: adelaidm
 */ 

#include "slider.h"
#include "adc.h"



SLI_pos_t SLI_getSliderPosition(void){
    return (SLI_pos_t){
        .L = ADC_read(SLIDER_L),
        .R = ADC_read(SLIDER_R)
    };
}
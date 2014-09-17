
#include "slider.h"
#include "adc.h"


SLI_pos_t SLI_getSliderPosition(void){
    return (SLI_pos_t){
        .L = ADC_read(SLIDER_L),
        .R = ADC_read(SLIDER_R)
    };
}
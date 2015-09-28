
#include <avr/io.h>

#include "adc.h"
#include "../memory_layout.h"

void adc_init(void){
    extern void memory_layout_init(void);
    memory_layout_init();
    DDRB &= ~(1<<DDB3);
}    

uint8_t ADC_read(ADC_channel ch){
    *ext_adc = ch;
    while(PINB & (1<<PINB3)){}
    return *ext_adc;
}
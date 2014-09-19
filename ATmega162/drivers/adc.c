
//#include "../config.h"
//#include <util/delay.h>
#include <avr/io.h>

#include "adc.h"
#include "memory_layout.h"

void __attribute__ ((constructor)) adc_init(void){    
    DDRB &= ~(1<<DDB2);
}    

uint8_t ADC_read(ADC_channel ch){
    
    ext_adc[1] = ch;
    //_delay_us(40);
    while(PINB & (1<<PINB2)){}
    return ext_adc[0];
}
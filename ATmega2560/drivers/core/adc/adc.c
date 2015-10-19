#include "adc.h"

#include <avr/io.h>

void adc_init(void){
    ADCSRA  |=  (1<<ADEN)
            |   (1<<ADPS2)  // Clock prescaler to 128x
            |   (1<<ADPS1)
            |   (1<<ADPS0);
}

uint16_t adc_read(ADC_channel ch){

    ADMUX = (1 << REFS0) | (ch & 0x1f);

    ADCSRA |= (1<<ADSC);
    
    while( (ADCSRA & (1<<ADSC)) ){}
    
    return ADC;
}
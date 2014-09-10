/*
 * adc.c
 *
 * Created: 10.09.2014 13:39:35
 *  Author: adelaidm
 */ 
#define F_CPU 4195200

#include "adc.h"
#include <util/delay.h>


char ADC_read(ADC_channel ch){
    volatile char* ext_adc = (char*) 0x1400; // Start address for the ADC
    
    ext_adc[1] = ch;
    _delay_us(50);
    return ext_adc[0];
}
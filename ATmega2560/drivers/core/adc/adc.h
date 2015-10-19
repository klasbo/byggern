#pragma once

#include <stdint.h>


typedef enum{
    IR_LED = 0,
} ADC_channel;

void adc_init(void);
uint16_t adc_read(ADC_channel ch);
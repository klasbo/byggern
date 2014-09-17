
#pragma once

typedef enum{
    JOY_X       = 4,
    JOY_Y       = 5,
    SLIDER_L    = 6,
    SLIDER_R    = 7
} ADC_channel;

uint8_t ADC_read(ADC_channel channel);


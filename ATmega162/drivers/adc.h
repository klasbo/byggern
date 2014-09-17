/*
 * adc.h
 *
 * Created: 10.09.2014 13:39:23
 *  Author: adelaidm
 */ 


#ifndef ADC_H_
#define ADC_H_

typedef enum{
    JOY_X       = 4,
    JOY_Y       = 5,
    SLIDER_L    = 6,
    SLIDER_R    = 7
} ADC_channel;

char ADC_read(ADC_channel channel);



#endif /* ADC_H_ */
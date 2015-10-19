
#include "ir_led.h"
#include "../core/adc/adc.h"

static uint16_t obstruct_threshold = 10;


void ir_obstruct_threshold(uint16_t t){
    obstruct_threshold = t;
}

uint8_t ir_obstructed(void){
    #define FILTER_ORDER 4
    #if FILTER_ORDER == 1
        return adc_read(IR_LED) < obstruct_threshold;
    #else
        static uint16_t filter[FILTER_ORDER-1];

        uint16_t val = adc_read(IR_LED);
        uint16_t avg = val;

        for(uint8_t i = 0; i < FILTER_ORDER-2; i++){
            avg += filter[i];
            filter[i] = filter[i+1];
        }
        avg += filter[FILTER_ORDER-2];
        filter[FILTER_ORDER-2] = val;

        return avg/FILTER_ORDER < obstruct_threshold;
    #endif
}

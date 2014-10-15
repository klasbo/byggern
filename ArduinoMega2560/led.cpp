#include <Arduino.h>
#include "led.h"
#include <stdint.h>

void LED_init(){
    pinMode(analogLed, INPUT);
}

uint8_t LED_read(void){
    //printf("analog led %d", analogRead(analogLed));
    return (analogRead(analogLed) > threshold);
}

uint8_t LED_score_count(){
    static uint8_t score_count;
    static uint8_t last_val=LED_read();

    if (!LED_read() && last_val){
        last_val=LED_read();
        return ++score_count;
    }
    last_val=LED_read();
    return score_count;
}
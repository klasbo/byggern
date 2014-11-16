
#include "ir.h"

#include <Arduino.h>
#include <stdint.h>

void IR_init(){
    pinMode(analogLed, INPUT);
}

uint8_t IR_obstructed(void){
    //printf("analog led %d", analogRead(analogLed));
    return (analogRead(analogLed) < threshold);
}
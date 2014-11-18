
#include "ir.h"

#include <Arduino.h>
#include <stdint.h>

void IR_init(){
    pinMode(IRPin, INPUT);
}

uint8_t IR_obstructed(void){
    return (analogRead(IRPin) < threshold);
}
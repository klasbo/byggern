
#include <avr/io.h>


void solenoid_init(void){
    DDRH |= (1 << DDH4);
}

void solenoid(uint8_t enable){
    enable ?
        (PORTH |= (1 << PH4)) :
        (PORTH &= ~(1 << PH4));
}

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "../lib/can/can.h"
#include "../lib/uart/uart.h"


extern void can_test(void);


int main(void){
    uart_init();
    can_init();

    can_test();

    while(1){}
}

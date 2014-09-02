#define F_CPU 4195200

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "drivers/uart.h"

int main(void)
{
	/*
	DDRB = 0x1;
    while(1)
    {
		
        PORTB |= (1<<PB0); 
		_delay_ms(2000);
		PORTB &= ~(1<<PB0);
		_delay_ms(2000);
		
    }
	*/
    
    	
	UART_init(31); //lol magic (FOSC/16/BAUD-1)
    fdevopen(&UART_transmit, &UART_receive);


    int i = 0;
    int j;

	while(1){
		// UART_transmit(UART_receive());
        // UART_transmit('\n');

        
        scanf("%d", &j);
        i += j;
        printf("i = %d\n", i);
        
	}
}
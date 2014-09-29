
#include <avr/io.h>
#include <stdio.h>

#include "uart.h"
#include "../../config.h"

#define ubrr (F_CPU/16/UART_BAUD - 1)

void __attribute__ ((constructor)) uart_init(void){
    //UBRR0H = (unsigned char)(ubrr>>8);      //not needed(?), see USCR0C
    UBRR0L = (unsigned char)(ubrr);

    // USART Control and Status
    UCSR0B =    (1<<RXEN0)      // receive enable
            |   (1<<TXEN0);     // transmit enable

    UCSR0C =    (1<<URSEL0)     // use UCSRC on I/O (as opposed to baud rate register high byte (UBBRH))
            |   (3<<UCSZ00);    // char size to 8


    fdevopen((int (*)(char, struct __file*))&UART_transmit, (int (*)(struct __file*))&UART_receive);
}

char UART_receive(void){
    while( !(UCSR0A & (1<<RXC0)) ){}    // busy-wait until receive complete

    return UDR0;
}

void UART_transmit(unsigned char c){
    while( !(UCSR0A & (1<<UDRE0)) ){}   // busy-wait until data register empty

    UDR0 = c;
}
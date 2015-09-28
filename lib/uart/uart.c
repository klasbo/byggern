
#include <avr/io.h>
#include <stdio.h>

#include "uart.h"

#define ubrr (F_CPU/16/BAUD - 1)

static FILE uart_stdio = FDEV_SETUP_STREAM(&uart_transmit, &uart_receive, _FDEV_SETUP_RW);

void uart_init(void){
    UBRR0L = (uint8_t)(ubrr);

    // USART Control and Status
    UCSR0B  |=  (1<<RXEN0)      // receive enable
            |   (1<<TXEN0);     // transmit enable

    UCSR0C  |=  (1<<URSEL0)     // use UCSRC on I/O (as opposed to baud rate register high byte (UBBRH))
            |   (3<<UCSZ00);    // char size to 8


    stdout = &uart_stdio;
    stdin = &uart_stdio;
}

uint8_t uart_receive(void){
    while( !(UCSR0A & (1<<RXC0)) ){}    // busy-wait until receive complete

    return UDR0;
}

void uart_transmit(uint8_t c){
    while( !(UCSR0A & (1<<UDRE0)) ){}   // busy-wait until data register empty

    UDR0 = c;
}
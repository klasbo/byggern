
#include <avr/io.h>
#include <stdio.h>

#include "uart.h"
#include "../../config.h"

#define ubrr (F_CPU/16/UART_BAUD - 1)

void __attribute__ ((constructor)) uart_init(void){
    UBRR1 = (unsigned char)(ubrr);

    // USART Control and Status
    UCSR1B  |=  (1<<RXEN1)      // receive enable
            |   (1<<TXEN1);     // transmit enable

    UCSR1C  |=  (3<<UCSZ10);    // char size to 8


    fdevopen((int (*)(char, struct __file*))&UART_transmit, (int (*)(struct __file*))&UART_receive);
}

char UART_receive(void){
    while( !(UCSR1A & (1<<RXC1)) ){}    // busy-wait until receive complete

    return UDR1;
}

void UART_transmit(unsigned char c){
    while( !(UCSR1A & (1<<UDRE1)) ){}   // busy-wait until data register empty

    UDR1 = c;
}
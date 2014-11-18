#include "uart.h"

#include "Arduino.h"
#include "stdio.h"

#define ubrr (F_CPU/16/9600 - 1)

int UART_receive(struct __file* unused){
    while( !(UCSR0A & (1<<RXC0)) ){}    // busy-wait until receive complete

    return UDR0;
}

int UART_transmit(char c, struct __file* unused){
    while( !(UCSR0A & (1<<UDRE0)) ){}   // busy-wait until data register empty

    UDR0 = c;
    return 1;
}

void uart_init(void){

    UBRR0L = (unsigned char)(ubrr);

    // USART Control and Status
    UCSR0B =    (1<<RXEN0)      // receive enable
    |   (1<<TXEN0);     // transmit enable

    UCSR0C =    (3<<UCSZ00);    // char size to 8

    fdevopen(&UART_transmit, &UART_receive);
}
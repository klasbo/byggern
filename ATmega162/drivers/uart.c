#include "uart.h"
#include <avr/io.h>

void UART_init(uint16_t ubrr){
    //UBRR0H = (unsigned char)(ubrr>>8);      //not needed(?), see USCR0C
    UBRR0L = (unsigned char)(ubrr);

    // USART Control and Status
    UCSR0B =    (1<<RXEN0)      // receive enable
            |   (1<<TXEN0);     // transmit enable

    UCSR0C =    (1<<URSEL0)     // use UCSRC on I/O (as opposed to baud rate register high byte (UBBRH))
            |   (3<<UCSZ00);    // char size to 8
}

char UART_receive(void){
    while( !(UCSR0A & (1<<RXC0)) ){}    // busy-wait until receive complete

    return UDR0;
}

void UART_transmit(unsigned char c){
    while( !(UCSR0A & (1<<UDRE0)) ){}   // busy-wait until data register empty

    UDR0 = c;
}
#pragma once


void UART_init(unsigned int ubrr);

char UART_receive(void);

void UART_transmit(unsigned char c);

#pragma once

#include <stdint.h>


/**
    Automatically hooks printf
    Make sure F_CPU and BAUD are defined as symbols (use flags for the compiler)
*/
void uart_init();

uint8_t uart_receive(void);

void uart_transmit(uint8_t c);

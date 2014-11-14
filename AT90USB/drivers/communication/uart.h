#pragma once

/**
Importing this module should not be necessary.
The module constructor sets the stdin/stdout/stderr streams to use the two functions below.
Prefer using printf/printf_P and scanf/scanf_P directly.
*/
char UART_receive(void);

void UART_transmit(unsigned char c);

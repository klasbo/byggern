
#pragma once



//TODO: rename(?)
void spi_chipselect(uint8_t enable);

//TODO: rename to send/recv(?)
void spi_write(uint8_t c);

uint8_t spi_read(void);

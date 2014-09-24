
#pragma once



//TODO: rename(?)
void SPI_chipselect(uint8_t enable);

//TODO: rename to send/recv(?)
void SPI_write(uint8_t c);

uint8_t SPI_read(void);


#pragma once



void SPI_chipselect(uint8_t enable);

void __attribute__ ((constructor)) SPI_init(void);

void SPI_write(uint8_t c);

uint8_t SPI_read(void);

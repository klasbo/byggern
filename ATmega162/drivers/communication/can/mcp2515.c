
#include <stdint.h>

#include "mcp2515.h"
#include "spi.h"

#define MCP_READ                0b00000011
#define MCP_WRITE               0b00000010
#define MCP_REQUEST_TO_SEND     0b10000000
#define MCP_READ_STATUS         0b10100000
#define MCP_BIT_MODIFY          0b00000101
#define MCP_RESET               0b11000000

uint8_t mcp2515_read(uint8_t addr){
    SPI_chipselect(1);

    SPI_write(MCP_READ);
    SPI_write(addr);
    uint8_t val = SPI_read();

    SPI_chipselect(0);

    return val;
}

void mcp2515_write(uint8_t val, uint8_t addr){
    SPI_chipselect(1);

    SPI_write(MCP_WRITE);
    SPI_write(addr);
    SPI_write(val);

    SPI_chipselect(0);
}

void mcp2515_request_to_send(uint8_t cmd){
    SPI_chipselect(1);

    SPI_write(MCP_REQUEST_TO_SEND | (cmd & 7));

    SPI_chipselect(0);
}

uint8_t mcp2515_read_status(void){
    SPI_chipselect(1);

    SPI_write(MCP_READ_STATUS);
    uint8_t val = SPI_read();
    
    SPI_chipselect(0);

    return val;
}

void mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t val){
    SPI_chipselect(1);

    SPI_write(MCP_BIT_MODIFY);
    SPI_write(addr);
    SPI_write(mask);
    SPI_write(val);

    SPI_chipselect(0);
}


void mcp2515_reset(void){
    SPI_chipselect(1);

    SPI_write(MCP_RESET);

    SPI_chipselect(0);
}



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
    spi_chipselect(1);

    spi_write(MCP_READ);
    spi_write(addr);
    uint8_t val = spi_read();

    spi_chipselect(0);

    return val;
}

void mcp2515_write(uint8_t val, uint8_t addr){
    spi_chipselect(1);

    spi_write(MCP_WRITE);
    spi_write(addr);
    spi_write(val);

    spi_chipselect(0);
}

void mcp2515_request_to_send(uint8_t cmd){
    spi_chipselect(1);

    spi_write(MCP_REQUEST_TO_SEND | (cmd & 7));

    spi_chipselect(0);
}

uint8_t mcp2515_read_status(void){
    spi_chipselect(1);

    spi_write(MCP_READ_STATUS);
    uint8_t val = spi_read();
    
    spi_chipselect(0);

    return val;
}

void mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t val){
    spi_chipselect(1);

    spi_write(MCP_BIT_MODIFY);
    spi_write(addr);
    spi_write(mask);
    spi_write(val);

    spi_chipselect(0);
}


void mcp2515_reset(void){
    spi_chipselect(1);

    spi_write(MCP_RESET);

    spi_chipselect(0);
}


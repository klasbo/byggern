
#pragma once

#include <stdint.h>


#define MCP_TXB0_SIDH   0x31
#define MCP_TXB0_SIDL   0x32
#define MCP_TXB0_DLC    0x35
#define MCP_TXB0_D0     0x36

#define MCP_RXB0_SIDH   0x61
#define MCP_RXB0_SIDL   0x62
#define MCP_RXB0_DLC    0x65
#define MCP_RXB0_D0     0x66

#define MCP_RTS_TXB0    0b00000001

#define MCP_RXB0CTRL    0x60
#define MCP_RXB0CTRL__FILTERS_OFF       0b01100000, 0xff
#define MCP_RXB0CTRL__ROLLOVER_OFF      0b00000100, 0x00

// TODO: this is terrible:
#define MCP_TXB0CTRL    0x30
#define MCP_TXB0CTRL__TX_REQUEST        0b00001000

#define MCP_CANCTRL     0x0f
#define MCP_CANCTRL__MODE_NORMAL        0b11100000, 0b00000000
#define MCP_CANCTRL__MODE_LOOPBACK      0b11100000, 0b01000000
#define MCP_CANCTRL__MODE_CONFIG        0b11100000, 0b10000000

#define MCP_CANINTE     0x2b
#define MCP_CANINTE__RX0_FULL_ENABLE    0b00000001, 0xff
#define MCP_CANINTE__RX0_FULL_DISABLE   0b00000001, 0x00

#define MCP_CANINTF     0x2c
#define MCP_CANINTF__RX0_CLEAR          0b00000001, 0x00


uint8_t mcp2515_read(uint8_t addr);

void mcp2515_write(uint8_t val, uint8_t addr);

void mcp2515_request_to_send(uint8_t cmd);

uint8_t mcp2515_read_status(void);

void mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t val);

void mcp2515_reset(void);



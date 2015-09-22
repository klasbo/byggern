
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
#define MCP_RXB0CTRL__SET_FILTERS_OFF       0x60, 0b01100000, 0xff
#define MCP_RXB0CTRL__SET_ROLLOVER_OFF      0x60, 0b00000100, 0x00

#define MCP_TXB0CTRL    0x30
#define MCP_TXB0CTRL__TEST_TX_REQUEST       0x30, 0b00001000

#define MCP_CANCTRL     0x0f
#define MCP_CANCTRL__SET_MODE_NORMAL        0x0f, 0b11100000, 0b00000000
#define MCP_CANCTRL__SET_MODE_LOOPBACK      0x0f, 0b11100000, 0b01000000
#define MCP_CANCTRL__SET_MODE_CONFIG        0x0f, 0b11100000, 0b10000000

#define MCP_CANINTE     0x2b
#define MCP_CANINTE__SET_RX0_FULL_ENABLE    0x2b, 0b00000001, 0xff
#define MCP_CANINTE__SET_RX0_FULL_DISABLE   0x2b, 0b00000001, 0x00

#define MCP_CANINTF     0x2c
#define MCP_CANINTF__SET_RX0_CLEAR          0x2c, 0b00000001, 0x00
#define MCP_CANINTF__TEST_RX0_FULL          0x2c, 0b00000001


uint8_t mcp2515_read(uint8_t addr);

static inline uint8_t mcp2515_bit_test(uint8_t addr, uint8_t val){
    return (mcp2515_read(addr) & val);
}

void mcp2515_write(uint8_t val, uint8_t addr);

void mcp2515_request_to_send(uint8_t cmd);

uint8_t mcp2515_read_status(void);

void mcp2515_bit_modify(uint8_t addr, uint8_t mask, uint8_t val);

void mcp2515_reset(void);



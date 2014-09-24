
#include "can.h"

#include "../mcp2515/mcp2515.h"


void __attribute__ ((constructor)) CAN_init(void){
    
    extern void SPI_init(void);
    SPI_init();

    mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL__FILTERS_OFF);
    mcp2515_bit_modify(MCP_RXB0CTRL, MCP_RXB0CTRL__ROLLOVER_OFF);

    mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL__MODE_LOOPBACK);

    mcp2515_bit_modify(MCP_CANINTE, MCP_CANINTE__RX0_FULL_ENABLE);
}

void CAN_send(can_msg_t msg){

    while((mcp2515_read(MCP_TXB0CTRL) & MCP_TXB0CTRL__TX_REQUEST)){}

    mcp2515_write(msg.ID >> 3, MCP_TXB0_SIDH);
    mcp2515_write(msg.ID << 5, MCP_TXB0_SIDL);
    mcp2515_write(msg.length, MCP_TXB0_DLC);
    for(int i = 0; i < msg.length; i++){
        mcp2515_write(msg.data[i], MCP_TXB0_D0 + i);
    }
    mcp2515_request_to_send(MCP_RTS_TXB0);
}

can_msg_t CAN_recv(void){
    can_msg_t msg;


    msg.ID = (mcp2515_read(MCP_RXB0_SIDH) << 3) | (mcp2515_read(MCP_RXB0_SIDL) >> 5);
    msg.length = (mcp2515_read(MCP_RXB0_DLC)) & (0x0f);
    for(int i = 0; i < msg.length; i++){
        msg.data[i] = mcp2515_read(MCP_RXB0_D0 + i);
    }
    for(int i = msg.length; i < 8; i++){
        msg.data[i] = 0;
    }

    mcp2515_bit_modify(MCP_CANINTF, MCP_CANINTF__RX0_CLEAR);

    return msg;
}
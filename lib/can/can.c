
#include "can.h"
#include "mcp2515.h"

#include <string.h>
#include <stdio.h>


void can_init(void){
    extern void spi_init(void);
    spi_init();

    mcp2515_reset();

    mcp2515_bit_modify(MCP_RXB0CTRL__SET_FILTERS_OFF);
    mcp2515_bit_modify(MCP_RXB0CTRL__SET_ROLLOVER_OFF);

    mcp2515_bit_modify(MCP_CANINTE__SET_RX0_FULL_ENABLE);
    
    mcp2515_bit_modify(MCP_CANCTRL__SET_MODE_NORMAL);
}

void can_send(can_msg_t msg){
    while(mcp2515_bit_test(MCP_TXB0CTRL__TEST_TX_REQUEST)){}

    mcp2515_write(msg.id >> 3,  MCP_TXB0_SIDH);
    mcp2515_write(msg.id << 5,  MCP_TXB0_SIDL);
    mcp2515_write(msg.length,   MCP_TXB0_DLC);
    for(int i = 0; i < msg.length; i++){
        mcp2515_write(msg.data.bytes[i], MCP_TXB0_D0 + i);
    }
    mcp2515_request_to_send(MCP_RTS_TXB0);
}

can_msg_t can_recv(void){

    can_msg_t msg;

    memset(&msg, 0, sizeof(can_msg_t));


    if(mcp2515_bit_test(MCP_CANINTF__TEST_RX0_FULL)){

        msg.id      = (mcp2515_read(MCP_RXB0_SIDH) << 3) | (mcp2515_read(MCP_RXB0_SIDL) >> 5);
        msg.length  = (mcp2515_read(MCP_RXB0_DLC)) & (0x0f);
        for(int i = 0; i < msg.length; i++){
            msg.data.bytes[i] = mcp2515_read(MCP_RXB0_D0 + i);
        }

        mcp2515_bit_modify(MCP_CANINTF__SET_RX0_CLEAR);
    } else {
        msg.id = CANID_None;
    }

    return msg;
}



void can_printmsg(can_msg_t m){
    printf("can_msg_t(id:%d, length:%d, data:{", m.id, m.length);
    if(m.length){
        printf("%d", m.data.bytes[0]);
    }
    for(uint8_t i = 1; i < 8; i++){
        printf(", %d", m.data.bytes[i]);
    }
    printf("})\n");
}
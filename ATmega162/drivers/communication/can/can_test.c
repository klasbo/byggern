
#include "../../../config.h"

#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "can.h"
#include "mcp2515.h"



void can_test(void){

    mcp2515_reset();
    mcp2515_bit_modify(MCP_CANCTRL, MCP_CANCTRL__SET_MODE_LOOPBACK);
    
    printf("\nMCP_CANCTRL: %x\n", mcp2515_read(MCP_CANCTRL));
    
    can_msg_t msg1;
    can_msg_t msg2;
    msg1.ID = 9;
    msg1.length = 5;
    for(int i = 0; i < 8; i++){
        msg1.data[i] = i*i;
    }

    while(1){
        CAN_send(msg1);
        msg2 = CAN_recv();
        printf_P(PSTR("msg1: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n"),
            msg1.ID,
            msg1.length,
            msg1.data[0], msg1.data[1], msg1.data[2], msg1.data[3],
            msg1.data[4], msg1.data[5], msg1.data[6], msg1.data[7]
        );
        printf_P(PSTR("msg2: can_msg_t(id:%d, len:%d, data:(%d, %d, %d, %d, %d, %d, %d, %d))\n\n"),
            msg2.ID,
            msg2.length,
            msg2.data[0], msg2.data[1], msg2.data[2], msg2.data[3],
            msg2.data[4], msg2.data[5], msg2.data[6], msg2.data[7]
        );
        msg1.data[0]++;
        
        _delay_ms(1000);
    }
}

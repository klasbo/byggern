

#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "can.h"
#include "can_types.h"
#include "mcp2515.h"
#include "../macros.h"


void can_test_loopback(void){

    mcp2515_bit_modify(MCP_CANCTRL__SET_MODE_LOOPBACK);
    
    printf("MCP_CANCTRL: 0x%02x (should be 0x47)\n", mcp2515_read(MCP_CANCTRL));
    printf("MCP_CANSTAT: 0x%02x (should be 0x40)\n", mcp2515_read(MCP_CANSTAT));
    
    can_msg_t msg1;
    can_msg_t msg2;
    msg1.id = 9;
    msg1.length = 5;
    for(int i = 0; i < 8; i++){
        msg1.data.bytes[i] = i*i;
    }

    while(1){
        can_sendmsg(msg1);
        can_printmsg(msg1);

        msg2 = can_recvmsg();
        can_printmsg(msg2);

        msg1.data.bytes[0]++;
        
        _delay_ms(1000);
    }
}



typedef struct EchoRequest EchoRequest;
struct EchoRequest {
    uint8_t from;
    uint8_t num;
} __attribute__((packed));

typedef struct EchoReply EchoReply;
struct EchoReply {
    uint8_t from;
    uint8_t to;
    uint8_t num;
} __attribute__((packed));

void can_test_normal_reply(uint8_t id){
    printf("Listening for echo requests...\n");

    mcp2515_bit_modify(MCP_CANCTRL__SET_MODE_NORMAL);

    while(1){
        can_receive( block,
            EchoRequest, er, {
                printf("Received EchoRequest(from:%d, num:%d)\n",
                    er.from, er.num
                );

                can_send(EchoReply, {
                    .from   = id,
                    .to     = er.from,
                    .num    = er.num
                });
            },
            default, msg, {
                printf("Received other: ");
                can_printmsg(msg);
            }
        );
    }
}

void can_test_normal_request(uint8_t id){
    printf("Listening for echo replies...\n");
    
    mcp2515_bit_modify(MCP_CANCTRL__SET_MODE_NORMAL);

    uint8_t msgNum = 1;
    uint8_t iter = 0;
    uint8_t n = 100;

    while(1){
        _delay_ms(10);

        // Check for replies
        can_receive(nonblock,
            EchoReply, er, {
                if(er.to == id){
                    printf("Received EchoReply(from:%d, to:%d, num:%d)\n",
                        er.from, er.to, er.num
                    );
                }
            }
        );

        // Send new echo request every n iterations
        iter++;
        if(iter == n){
            iter = 0;
            msgNum++;

            printf("Sending new echo request...\n");
            can_send(EchoRequest, {
                .from = id,
                .num = msgNum,
            });
        }

    }
}
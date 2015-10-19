
#pragma once

#include <stdint.h>
#include "../macros.h"

typedef struct Byte8 Byte8;
struct Byte8 {
    uint8_t bytes[8];
};

typedef struct can_msg_t can_msg_t;
struct can_msg_t {
    uint8_t     id;
    uint8_t     length;
    Byte8       data;
};


void can_init(void);

void can_send(can_msg_t msg);

can_msg_t can_recv(void);

void can_printmsg(can_msg_t m);



// ID of some invalid message
// Equal to "typeof(can_msg_t::id).max"
#define CANID_None (typeof( member(can_msg_t, id) ))(-1)

#pragma once

#include <stdint.h>

typedef struct can_msg_t can_msg_t;

struct can_msg_t {
    uint8_t     ID;
    uint8_t     length;
    uint8_t     data[8];
};



void CAN_send(can_msg_t msg);

can_msg_t CAN_recv(void);
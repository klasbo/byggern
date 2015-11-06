
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


// ID of an invalid message
// Equal to "typeof(can_msg_t::id).max"
#define CANID_None (typeof( member(can_msg_t, id) ))(-1)


// Initialize
void can_init(void);




// Send an automatically-packed message
// varargs just to allow commas in initializer
/* Examples:

    can_send(uint8_t, 5);

    struct {
        uint8_t i;
        uint8_t j;
    } SomeStruct;
    can_send(SomeStruct, {.j = 4, .i = 3});

    // Can also use an existing value as the initializer
    SomeStruct ss = (SomeStruct){.i = 5, .j = 3};
    can_send(SomeStruct, ss);
*/
#define can_send(type, initializer, ...) \
    can_sendmsg(can_pack(type, initializer, __VA_ARGS__));


// Send a can_msg_t struct.
// Prefer using can_send for most applications
void can_sendmsg(can_msg_t msg);


// Pack (almost) any type into a can_msg_t
// varargs just to allow commas in initializer
#define can_pack(type, initializer, ...) \
    ({ \
    _Static_assert(sizeof(type) <= sizeof(Byte8), "Cannot pack messages larger than 8 bytes"); \
    (can_msg_t){ \
        .id = typeid(type), \
        .length = sizeof(type), \
        .data = union_cast(Byte8, (type)\
            MAP(IDENTITY, COMMA, initializer, __VA_ARGS__) \
        ), \
    }; \
    })



    



// Receive automatically-packed messages, and pattern-match to types
// varargs must come in {type, identifier, statement} triplets
// Arguments:
//  blocking:   Either 'block' or 'nonblock'. 'block' will poll until a CAN message is received.
//  type:       The type of the pattern
//  identifier: The name of the variable as used in statement
//  statement:  Block of statements to run when the received message matches the type
// If 'default' is passed as the final type, it will match any message not matched earlier
//   The type of identifier will then be 'can_msg_t'
/* Example

    struct {
        uint8_t i;
        uint8_t j;
    } SomeStruct;

    can_receive(block,
        int, i, {
            printf("Received int: %d\n", i);
        },
        SomeStruct, ss, {
            printf("Received SomeStruct(i:%d, j:%d)\n", ss.i, ss.j);
        },
        default, msg, {
            printf("Received other message:\n");
            can_printmsg(msg);
        }
    );
*/
#define can_receive(blocking, type, identifier, statement, ...) \
    { \
        can_msg_t _msg = {.id = CANID_None}; \
        if(blocking == block){ \
            while(_msg.id == CANID_None){ \
                _msg = can_recvmsg(); \
            } \
        } else { \
            _msg = can_recvmsg(); \
        } \
        if(0){ \
        MAP_TRIPLETS(can_receive_case, EMPTY, type, identifier, statement, __VA_ARGS__) \
        } \
    }


// Blocking type, as used by can_receive
typedef enum Blocking Blocking;
enum Blocking {
    nonblock = 0,
    block = 1
};


// Receive a can_msg_t struct
// Prefer using can_receive for most applications
// ::id will be CANID_None if there was no message to receive
can_msg_t can_recvmsg(void);





// Print a can_msg_t with printf
void can_printmsg(can_msg_t m);






// (Internal)
#define can_receive_case(type, identifier, statement) \
    IF_ELSE(IS_DEFAULT(type))( \
        } else { \
            can_msg_t identifier = _msg; \
            statement \
        , \
        } else if(_msg.id == typeid(type)) { \
            type identifier = union_cast(type, _msg.data); \
            statement \
    )



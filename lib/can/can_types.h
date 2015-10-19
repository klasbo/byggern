#include <stdint.h>

#include "../macros.h"

#define CANID_ControlCmd 50

typedef enum MotorControlType MotorControlType;
enum MotorControlType {
    MC_Speed,
    MC_Position,
};

typedef struct CanMsg_ControlCmd CanMsg_ControlCmd;
struct CanMsg_ControlCmd {
    struct {
        MotorControlType controlType;
        union {
            int8_t  speed;
            uint8_t position;
        };
    } motor;
    
    int8_t  servo;
    uint8_t solenoid;
} __attribute__((packed));





#define CANID_GameOver 51
// No length or data -> no type



#define CANID_EchoRequest  216
#define CANID_EchoReply    217

typedef struct CanMsg_Echo CanMsg_Echo;
struct CanMsg_Echo {
    uint8_t requestOrigin;
    uint8_t msgOrigin;
    uint8_t msgNum;
} __attribute__((packed));
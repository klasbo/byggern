#include <stdint.h>

#include "../macros.h"


typedef enum MotorControlType MotorControlType;
enum MotorControlType {
    MC_Speed,
    MC_Position,
};

typedef struct Pong_ControlCmd Pong_ControlCmd;
struct Pong_ControlCmd {
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





typedef struct Pong_GameOver Pong_GameOver;
struct Pong_GameOver {
    uint8_t v;
} __attribute__((packed));


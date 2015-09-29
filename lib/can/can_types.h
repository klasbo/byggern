#include <stdint.h>



#define CANMsg_ControlCmd 50

typedef enum MotorControlType MotorControlType;
enum MotorControlType {
    MC_Speed,
    MC_Position,
};

typedef struct ControlCmdMsg ControlCmdMsg;
struct ControlCmdMsg {
    struct {
        MotorControlType controlType;
        union {
            int8_t  speed;
            uint8_t position;
        }        
    } motor;
    
    int8_t  servoAngle;
    uint8_t solenoid;
};





#define CANMsg_GameOver 51

typedef uint8_t GameOverMsg;
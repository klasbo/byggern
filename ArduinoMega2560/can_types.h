typedef struct ControlCmd ControlCmd;
struct ControlCmd {
    uint8_t     servoPos;   // 0 - 180
    int8_t      motorSpeed; // -127 - 128
    uint8_t     solenoid;   // 0 (retracted) or 1 (GET PUNCHED)
    uint8_t     alignment[5];
};

#define CANID_ControlCmd 50
#define CANID_IRLED 3